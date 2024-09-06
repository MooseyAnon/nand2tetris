"""16bit hack assembler.

Spec:

a instructions start with 0
c instructions start with 1

example c instruction:

c_inst = 1 1 1 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
c_inst in text => dest = comp; jump

c_inst[0] = op code
c_inst[1 - 2] = not used
c_inst[3 - 9] = comp bits
c_inst[10 - 12] = dest bits
c_inst[13 - 15] = jump bits

comp bits table if a = 0:
0: 101010,
1: 111111,
-1: 111010,
"D": 001100,
"A": 110000,
"!D": 001101,
"!A": 110001,
"-D": 001111,
"-A": 110011,
"D+1": 011111,
"A+1": 110111,
"D-1": 001110,
"A-1": 110010,
"D+A": 000010,
"D-A": 010011,
"A-D": 000111,
"D&A": 000000,
"D|A": 010101,

comp bits table if a = 1:
"M": "A",
"!M": "!A",
"-M": "-A",
"M+1": "A+1",
"M-1": "A-1",
"D+M": "D+A",
"D-M": "D-A",
"M-D": "A-D",
"D&M": "D&A",
"D|M": "D|A",

dest bits table:
"null": 000,  # the value is not stored
"M": 001,     # RAM[A]
"D": 010,     # D register
"MD": 011,    # RAM[A] and D register
"A": 100,     # A register
"AM": 101,    # A register and RAM[A]
"AD": 110,    # A register and D register
"AMD": 111,   # A register, RAM[A], and D register

jump bits table:
"null": 000,  # no jump
"JGT": 001,   # if out > 0 jump
"JEQ": 010,   # if out == 0 jump
"JGE": 011,   # if out >= 0 jump
"JLT": 100,   # if our < 0 jump
"JNE": 101,   # if out != 0 jump
"JLE": 110,   # if out <= 0 jump
"JMP": 111,   # unconditional jump

symbol table for built in symbols:
"R0": 0,
"R1": 1,
"R2": 2,
"R3": 3,
"R4": 4,
"R5": 5,
"R6": 6,
"R7": 7,
"R8": 8,
"R9": 9,
"R10": 10,
"R11": 11,
"R12": 12,
"R13": 13,
"R14": 14,
"R15": 15,
"SCREEN": 16384,
"KBD": 24576,
"SP": 0,
"LCL": 1,
"ARG": 2,
"THIS": 3,
"THAT": 4,

"""
import argparse
import pathlib
import sys

import emit
import utils

# symbol tables
dest: dict[str, str] = {
    "null": "000",
    "M": "001",
    "D": "010",
    "MD": "011",
    "A": "100",
    "AM": "101",
    "AD": "110",
    "AMD": "111",
}

jump: dict[str, str] = {
    "null": "000",
    "JGT": "001",
    "JEQ": "010",
    "JGE": "011",
    "JLT": "100",
    "JNE": "101",
    "JLE": "110",
    "JMP": "111",
}

comp_a_is_0: dict[str, str] = {
    # when a == 0
    "0": "101010",
    "1": "111111",
    "-1": "111010",
    "D": "001100",
    "A": "110000",
    "!D": "001101",
    "!A": "110001",
    "-D": "001111",
    "-A": "110011",
    "D+1": "011111",
    "A+1": "110111",
    "D-1": "001110",
    "A-1": "110010",
    "D+A": "000010",
    "D-A": "010011",
    "A-D": "000111",
    "D&A": "000000",
    "D|A": "010101",
}


comp_a_is_1: dict[str, str] = {
    # when a == 1
    "M": "110000",
    "!M": "110001",
    "-M": "110011",
    "M+1": "110111",
    "M-1": "110010",
    "D+M": "000010",
    "D-M": "010011",
    "M-D": "000111",
    "D&M": "000000",
    "D|M": "010101",
}


symbols: dict[str, str] = {
    # registers on CPU
    "R0": "0",
    "R1": "1",
    "R2": "2",
    "R3": "3",
    "R4": "4",
    "R5": "5",
    "R6": "6",
    "R7": "7",
    "R8": "8",
    "R9": "9",
    "R10": "10",
    "R11": "11",
    "R12": "12",
    "R13": "13",
    "R14": "14",
    "R15": "15",

    # extra variables used by jack VM
    "SP": "0",
    "LCL": "1",
    "ARG": "2",
    "THIS": "3",
    "THAT": "4",

    # peripherals
    "SCREEN": "16384",
    "KBD": "24576",
}


globals = {
    "next_register": 16,
    # holds the count of valid lines of code i.e. not whitespace, comments etc
    "line_count": 0,
}


def is_comment(line, idx, len):
    return (idx + 1 < len) and (line[idx + 1] == "/")


def get_variable(line: str) -> str:
    """Parse an asm variable.

    According to the hack asm spec, variables start with the @ symbol.
    """
    if not line.startswith("@"):
        raise ValueError("Invalid Variable")

    var: list[str] = []

    for i in range(len(line)):
        char: str = line[i]

        if char == "@":
            continue

        # reached end of variable
        if char == " " or char == "\n":
            break

        # reached a comment
        if is_comment(line, i, len(line)):
            break

        # char is part of variable
        var.append(char)

    return "".join(var)


def get_symbol(line: str) -> str:
    """Parse an asm symbol.

    According to hack spec, all valid symbols start with '('
    and end with ')' and contain no spaces.
    """
    if not line.startswith("("):
        raise ValueError("Invalid variable definition")

    var: list[str] = []
    closed: bool = False

    for i in range(len(line)):
        char = line[i]

        if char == "(":
            continue

        # reached end of variable
        if char == ")":
            closed = True
            break

        # contains spaces and is invalid
        if char == " ":
            break

        # reached a comment
        if is_comment(line, i, len(line)):
            break

        # char is part of variable
        var.append(char)

    if not closed:
        raise ValueError("Invalid variable definition")

    return "".join(var)


def _parse_instruction(line):
    if not line:
        raise ValueError("Invalid instruction")

    # separate components of the instruction
    s = line.split("=")
    s2 = s[1].split(";") if len(s) == 2 else s[0].split(";")

    out = {
        "dest": s[0] if len(s) == 2 else "null",
        "comp": s2[0],
        "jump": s2[1] if len(s2) == 2 else "null",
    }

    return out


def instruction(line):
    out = _parse_instruction(line)

    x = ["1","1","1"]
    # byte order is comp, dest, jump
    if out["comp"] in comp_a_is_0:
        x.append("0")
        x.append(comp_a_is_0[out["comp"]])
    else:
        x.append("1")
        x.append(comp_a_is_1[out["comp"]])

    x.append(dest[out["dest"]])
    x.append(jump[out["jump"]])

    return "".join(x)


def variable(line):
    var = get_variable(line)

    # we want to convert the variable to a number
    # where this number comes from depends on whether the
    # variable is aleady a decimal number or if we need to
    # use the next available register as a proxy
    as_num = None

    if utils.is_number(var):
        as_num = utils.atoi(var)

    else:
        # look up in symbol table
        if var not in symbols:
            # convert to string to maintain consistency with the rest of our
            # symbol mapping
            symbols[var] = str(globals["next_register"])
            # increment the register after using
            globals["next_register"] += 1

        as_num = utils.atoi(symbols[var])

    # turn the decimal number to binary
    dec_bin = utils.dec_to_bin(as_num)

    return dec_bin


def assemble(fp):
    # build symbole table in a first pass
    build_symbol_table(fp)

    for line in utils.readlines(fp):
        fist_char_idx = utils.first(line)

        # -1 means its an empty str
        if fist_char_idx == -1: continue
        # skip labels as we've already accounted for them during symbol table
        # building first pass
        if line[fist_char_idx] == "(": continue

        # valid line of hack asm
        cleaned = utils.clean(line)

        # hadle variable
        if cleaned.startswith("@"):
            emit.emit(variable(cleaned))
        # handle instruction
        else:
            emit.emit(instruction(cleaned))


    # flush anything left in the buffer
    emit.flush()


def build_symbol_table(fp):
    for line in utils.readlines(fp):
        if utils.first(line) == -1: continue
        # valid line of hack asm
        line = utils.clean(line)

        if line.startswith("("):
            symbol = get_symbol(line)
            # set symbol to point to the next line
            # essentially ignore this symbol in our line count
            symbols[symbol] = str(globals["line_count"])

        else:
            globals["line_count"] += 1


def main():
    """Command line tool for assembling hack ASM to machine code."""

    parser = argparse.ArgumentParser(
        prog="Assemble", description="N2T Hack Assembler")

    parser.add_argument(
        "file",
        type=pathlib.Path,
        help="Path to file containing hack. Note: You must have read "
             "privileges on said file."
    )

    parser.add_argument(
        "--out",
        type=pathlib.Path,
        default="out.hack",
        help="file to write output to."
    )

    parser.add_argument(
        "--symbols-only",
        action="store_true",
        help="Print the symbol table to the console."
    )

    return parser.parse_args()


if __name__ == "__main__":
    args = main()

    if not args.file.exists():
        print("FILE DOES NOT EXIST!!")
        sys.exit(1)

    if args.symbols_only:
        build_symbol_table(args.file)
        print(symbols)

    else:
        # initialize .hack file
        emit.init(args.out)

        assemble(args.file)
        print("Successfully assembled!!")

    sys.exit(0)
