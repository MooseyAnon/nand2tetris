"""Utility functions."""


def readlines(fp: str) -> str:

    with open(fp, "r") as fd:
        # walrus op: https://stackoverflow.com/q/50297704
        while line := fd.readline():
            yield line


def _is_number(char: str) -> bool:
    """Check if a character is a decimal number.

    Note: this assumes the character is ASCII.
    """
    return ord("0") <= ord(char) <= ord("9")


def is_number(chars: str) -> bool:
    """Check if a string of characters is made up of decimal numbers.

    Note: this assumes ASCII characters.
    """
    if not chars: return False

    # check if we have a negative number
    start_index: int = 1 if chars[0] == "-" else 0

    result: bool = True

    for i in range(start_index, len(chars)):
        if not _is_number(chars[i]):
            result = False
            break

    return result


def atoi(string: str) -> int:
    """Turn a given string to a number.

    Note: Only works with decimal numbers.
    """
    sign: int = -1 if string[0] == "-" else 1
    start_index: int = 1 if sign == -1 else 0

    result: int = 0

    for i in range(start_index, len(string)):
        char: str = string[i]

        if not _is_number(char):
            raise ValueError("All characters in string must be decimal")

        result *= 10
        result += int(char)

    return result * sign


def dec_to_bin(number: int) -> str:
    """Covert a decimal number to binary.

    Note: the binary representation is a string as that is what is
    required for the assembler.

    Note 2: This only works on positive numbers (inc 0). To implement
    negatives use twos comp [1].

    [1] https://www.cs.cornell.edu/~tomf/notes/cps104/twoscomp.html
    [2] https://stackoverflow.com/a/12946226 - python twos comp
    """
    if not isinstance(number, int):
        raise ValueError("Input must be a decimal number")

    result: list[str] = []

    while number >= 1:
        # modding by 2 will give us either zero or one
        mod: int = number % 2
        result.append(str(mod))
        # divide by two to move down to the next power of 2
        number = number // 2

    # pad any spaces with 0s as we want all values to be 16bit
    while 16 > len(result):
        result.append("0")

    # result list contains the binary result but in reverse so
    # needs to be revered
    return "".join(result[::-1])


def bin_to_dec(binary: str) -> int:
    """Convert binary string to decimal integer.

    Calculation is as follows - for "101":
        := (1 * 2 ^ 2) + (0 * 2 ^ 1) + (1 * 2 ^ 0)
        := 4 + 0 + 1
        := 5

    probably a simpler calculation would be - for "101" [0]:
        result = 0
        := result * 2 + 1 = 1
        := result * 2 + 0 = 2
        := result * 2 + 1 = 5

    [0] found here: https://stackoverflow.com/a/21765799

    Note: this only works for positive integers, does not handle signed.
    """
    result: int = 0

    for i in range(len(binary)):
        # we need to do -1 because binary is 0 indexed, i.e. right most
        # value is 2 ^ 0 + i.
        # so if the length of the binary string is 5, the max pow we raise
        # 2 to is 2 ^ 4.
        exp: int = (len(binary) - i - 1)
        result += int(binary[i]) * (2 ** exp)

    return result


def first(line: str) -> int:
    """Get the first legitimate character of the currently line.

    A legitimate character is defined as the following:
        - None space character
        - Not a comment - "//"
        - Not a newline character - "\n"
    """
    first: int = -1

    for i in range(len(line)):
        char: str = line[i]

        # space character
        if char == " ":
            continue

        # empty line
        if char == "\n":
            break

        # comment
        if (
            char == "/"
            and (i + 1 < len(line) and line[i + 1] == "/")
        ):
            break

        else:
            first = i
            break

    return first


def clean(line: str) -> str:
    """Clean incoming asm.

    Clean means:
        - remove spaces before and after any characters.
        - remove any comments
        - remove any new line characters
     """
    inst: list[str] = []

    i: int = 0
    # skip spaces
    while i < len(line) and line[i] == " ":
        i += 1

    while i < len(line):
        char: str = line[i]

        # reached end of valid asm
        if char == " " or char == "\n":
            break

        # reached a comment
        if char == "/" and (i + 1 < len(line) and line[i + 1] == "/"):
            break

        inst.append(char)
        i += 1

    return "".join(inst)


def in_(line, symbol):
    for char in line:
        if char == symbol:
            return True

    return False


def split_instruction(inst):
    i = 0
    out = []

    while i < len(inst):
        curr = []

        while i < len(inst) and (inst[i] != "=" and inst[i] != ";"):
            curr.append(inst[i])
            i += 1

        out.append("".join(curr))
        i += 1

    return out
