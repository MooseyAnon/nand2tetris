#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "mystring.h"
#include "hash.h"


// ------------------ prototypes ------------
void push_callback(Item *, void *);

// ----------- ---- UTILS -----------------

int first(String str)
{
    int result = -1;

    for (size_t i = 0; i < str.length; i++) {
        char curr = charat(str, i);

        // space character, we can ignore
        if (curr == ' ')
            continue;

        // empty line
        if (curr == '\n')
            break;

        // comment
        if (
            curr == '/' &&
            (i + 1 < str.length && str.s[i + 1] == '/')
        )
            break;

        // we've found the first proper char
        result = (int)i;
        break;
    }

    return result;
}


String clean(String str)
{
    String ns = newstr(""); 
    size_t i = 0;

    while (i < str.length && str.s[i] == ' ')
        i++;

    while (i < str.length) {
        char curr = charat(str, i);

        // reached the end of valid asm
        // NOTE: this does not support space inbetween valid asm statements
        if (curr == ' ' || curr == '\n')
            break;

        // comment
        if (
            curr == '/' &&
            (i + 1 < str.length && str.s[i + 1] == '/')
        )
            break;

        ns = concatChar(ns, &curr);
        i++;
    }

    return ns;
}


String str_reverse(String str)
{
    String ns = newstr("");

    // cast to int as size_t doesn't go below 0
    int i = (int)str.length - 1;

    while (i >= 0) {
        char curr = charat(str, (size_t)i);
        ns = concatChar(ns, &curr);

        i--;
    }

    return ns;
}


String dec_to_str(unsigned int num)
{
    String ns = newstr("");

    char d[2];
    d[1] = '\0';

    while (num > 0) {
        char tmp = '0' + (num % 10);
        num /= 10;

        d[0] = tmp;
        ns = concatChar(ns, d);

    }

    return str_reverse(ns);
}


bool is_number(String str)
{
    if (str.s == NULL)
        return false;

    // check if number is signed
    size_t start_idx = (str.s[0] == '-') ? 1 : 0;

    for (size_t i = start_idx; i < str.length; i++) {
        char curr = charat(str, i);

        if (!(('0' <= curr) && (curr <= '9')))
            return false;
    }

    return true;
}


long myatoi(String str)
{
    // check if number is signed
    int sign = (str.s[0] == '-') ? -1 : 1;
    size_t start_idx = (sign == -1) ? 1 : 0;

    long result = 0;

    for (size_t i = start_idx; i < str.length; i++) {
        char curr = charat(str, i);

        result *= 10;
        result += (curr - '0');
    }

    return result * sign;
}


String dec_to_bin(long number)
{
    String result = newstr("");

    while (number >= 1) {
        // modding by 2 will give us either 0 or 1
        char mod = '0' + (number % 2);
        result = concatChar(result, &mod);
        // divide by two to move down to the next power of 2
        number /= 2;
    }

    // pad any space with 0s because we want 16bit values
    while ((result = concatChar(result, "0")).length < 16);

    return str_reverse(result);
}


Array * str_split(String str, char *symbol)
{
    Array *arr = newarr(push_callback);

    // ensure we have only one character to compare to
    char needle = symbol[0];

    // create a new string containing all letters up to the symbol
    String tmp = newstr("");

    for (size_t i = 0; i < str.length; i++) {
        char curr = charat(str, i);

        if (curr == needle) {

            if (tmp.length) {
                // we have a valid string
                push(arr, &tmp);
                // reset string
                tmp = newstr("");
            }

            continue;

        }

        tmp = concatChar(tmp, &curr);
    }

    // check if there is a string left over
    if (tmp.length)
        push(arr, &tmp);

    return arr;
}


void exit_with_messages(char *message)
{
    printf("%s\n", message);
    exit(1);
}


// ------------ GLOBAL CONSTANTS ----------

// holds the count of valid lines of code i.e. not whitespace, comments etc
size_t LINE_CNT = 0;
// next available register, starts at 16 as hack uses first 15 by default
unsigned int NXT_REG = 16;

// initialized at run time
static HT *DEST = NULL;     // op codes for destination of hack asm
static HT *JUMP = NULL;     // op codes for jump related asm
static HT *COMP_0 = NULL;   // op codes for when instruction starts with 0
static HT *COMP_1 = NULL;   // op codes for when instruction starts with 1
static HT *SYMBOLS = NULL;  // user defined symbols/variables

// ------------- Symbol tables ------------
// to make things easier we hard code the basic asm -> machine code symbols


HT * dest(void)
{
    HT *map = create();

    set(map, "null", "000");
    set(map, "M", "001");
    set(map, "D", "010");
    set(map, "MD", "011");
    set(map, "A", "100");
    set(map, "AM", "101");
    set(map, "AD", "110");
    set(map, "AMD", "111");

    return map;
}


HT * jump(void)
{
    HT *map = create();

    set(map, "null", "000");
    set(map, "JGT", "001");
    set(map, "JEQ", "010");
    set(map, "JGE", "011");
    set(map, "JLT", "100");
    set(map, "JNE", "101");
    set(map, "JLE", "110");
    set(map, "JMP", "111");

    return map;
}


HT * comp_zero(void)
{
    HT *map = create();

    set(map, "0", "101010");
    set(map, "1", "111111");
    set(map, "-1", "111010");
    set(map, "D", "001100");
    set(map, "A", "110000");
    set(map, "!D", "001101");
    set(map, "!A", "110001");
    set(map, "-D", "001111");
    set(map, "-A", "110011");
    set(map, "D+1", "011111");
    set(map, "A+1", "110111");
    set(map, "D-1", "001110");
    set(map, "A-1", "110010");
    set(map, "D+A", "000010");
    set(map, "D-A", "010011");
    set(map, "A-D", "000111");
    set(map, "D&A", "000000");
    set(map, "D|A", "010101");

    return map;
}


HT * comp_one(void)
{
    HT *map = create();

    set(map, "M", "110000");
    set(map, "!M", "110001");
    set(map, "-M", "110011");
    set(map, "M+1", "110111");
    set(map, "M-1", "110010");
    set(map, "D+M", "000010");
    set(map, "D-M", "010011");
    set(map, "M-D", "000111");
    set(map, "D&M", "000000");
    set(map, "D|M", "010101");

    return map;
}


HT * symbols(void)
{
    HT *map = create();

    // base registers on CPU
    set(map, "R0", "0");
    set(map, "R1", "1");
    set(map, "R2", "2");
    set(map, "R3", "3");
    set(map, "R4", "4");
    set(map, "R5", "5");
    set(map, "R6", "6");
    set(map, "R7", "7");
    set(map, "R8", "8");
    set(map, "R9", "9");
    set(map, "R10", "10");
    set(map, "R11", "11");
    set(map, "R12", "12");
    set(map, "R13", "13");
    set(map, "R14", "14");
    set(map, "R15", "15");

    // variables use by the jack VM
    set(map, "SP", "0");
    set(map, "LCL", "1");
    set(map, "ARG", "2");
    set(map, "THIS", "3");
    set(map, "THAT", "4");

    // peripherals
    set(map, "SCREEN", "16384");
    set(map, "KBD", "24576");

    return map;
}


void push_callback(Item *it, void *data)
{
    String *ndata = calloc(sizeof(ndata), 1);
    *ndata = *(String *)data;
    it->data = ndata;
}


Array * readlines(char *path)
{
    int c;
    // depending on how the file is saved e.g. as UTF-8, we can have
    // problems reading characters. For example, a byte order mark may be
    // automatically added to the first line, so characters come out as
    // gibberish. Rather than perfectly trying to decode any unicode character,
    // here we simpply make a one letter null terminated 'string' but casting
    // the incoming character and assigning its value to the first index of
    // this array.
    char d[2];

    Array *arr = newarr(push_callback);

    FILE *fp = fopen(path, "rb");

    String line = newstr("");

    d[1] = '\0';

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {

            push(arr, &line);
            line = newstr("");
            continue;
        }

        char c2 = (char)c;
        d[0] = c2;

        line = concatChar(line, d);
    }

    fclose(fp);

    return arr;

}


void write_hack(String bin)
{
    if (bin.length == 0)
        return;

    FILE *fp = fopen("out.hack", "a");

    fprintf(fp, "%s\n", bin.s);

    // cleanup
    fclose(fp);
    freestr(bin);
}


String get_symbol(String str)
{
    int closed = 0;
    String ns = newstr("");

    for (size_t i = 0; i < str.length; i++) {
        char curr = charat(str, i);

        if (curr == '(')
            continue;

        // end of variable
        // Note: this does not support spaces around brackets
        if (curr == ' ' || curr == '\n')
            break;

        // comment
        if (
            curr == '/' &&
            (i + 1 < str.length && str.s[i + 1] == '/')
        )
            break;

        if (curr == ')') {
            closed = 1;
            break;
        }

        ns = concatChar(ns, &curr);
    }

    if (!closed)
        exit_with_messages("Invalid variable definition");

    return ns;
}


String get_variable(String str)
{
    String ns = newstr("");

    for (size_t i = 0; i < str.length; i++) {
        char curr = charat(str, i);

        if (curr == '@')
            continue;

        // end of variable
        // Note: this does not support spaces between '@' and var name
        if (curr == ' ' || curr == '\n')
            break;

        // comment
        if (
            curr == '/' &&
            (i + 1 < str.length && str.s[i + 1] == '/')
        )
            break;

        ns = concatChar(ns, &curr);
    }

    return ns;
}


void build_symbol_table(Array *arr)
{
    for (size_t i = 0; i < arr->length; i++) {
        String line = *(String *)arr->items[i].data;

        if (first(line) == -1)
            continue;

        line = clean(line);

        if (startswith(line, "(")) {
            String symbol = get_symbol(line);
            const char *result = set(SYMBOLS, symbol.s, dec_to_str(LINE_CNT).s);

            if (result == NULL)
                printf("Did not save: %s", symbol.s);

        } else {
            LINE_CNT++;
        }

    }
}


HT * _instruction_parser(String line)
{
    Array *s;
    Array *s2;

    HT *map = create();

    // init values
    set(map, "dest", "null");
    set(map, "jump", "null");
    set(map, "comp", NULL);

    // Both jump and dest are optional but at least one of them has to be present
    // so this is a bit of a pain but necessary
    // Future changes should make this more robust to asm syntax errors.
    s = str_split(line, "=");
    s2 = str_split(*(String *)s->items[s->length == 2 ? 1 : 0].data, ";");

    // set comp
    String tmp = *(String *)s2->items[0].data;
    set(map, "comp", tmp.s);

    // set dest if present
    if (s->length == 2) {
        String curr = *(String *)s->items[0].data;
        set(map, "dest", curr.s);
    }

    // set jump if present
    if (s2->length == 2) {
        String curr = *(String *)s2->items[1].data;
        set(map, "jump", curr.s);
    }

    // cleanup
    freearray(s);
    freearray(s2);

    return map;
}


String instruction_parse(String line)
{
    HT *parsed = _instruction_parser(line);

    // holds the binary result of the instruction
    String result = newstr("111");

    char *comp = (char *)get(parsed, "comp");
    char *dst = (char *)get(parsed, "dest");
    char *jmp = (char *)get(parsed, "jump");

    if (!comp)
        printf("COMP NOT FOUND!\n");

    if (get(COMP_0, comp) != NULL) {
        result = concatChar(result, "0");
        result = concatRaw(result, (char *)get(COMP_0, comp));

    } else {
        result = concatChar(result, "1");
        result = concatRaw(result, (char *)get(COMP_1, comp));
    }

    result = concatRaw(result, (char *)get(DEST, dst));
    result = concatRaw(result, (char *)get(JUMP, jmp));

    // cleanup
    destroy(parsed);

    return result;
}


String variable(String str)
{
    long as_num;

    String var = get_variable(str);

    if (is_number(str)) {
        as_num = myatoi(var);

    } else {
        // look up in symbol table
        if (get(SYMBOLS, var.s) == NULL) {
            // convert to string to maintain consistency with the rest of our
            // symbol mapping
            set(SYMBOLS, var.s, dec_to_str(NXT_REG).s);
            // increment the register after using
            NXT_REG++;
        }

        char *sym = get(SYMBOLS, var.s);
        as_num = myatoi(newstr(sym));
    }

    String dec_bin = dec_to_bin(as_num);

    return dec_bin;
}


void assemble(Array *arr)
{
    // build the symbol table for user declared variables
    build_symbol_table(arr);

    for (size_t i = 0; i < arr->length; i++) {
        String line = *(String *)arr->items[i].data;

        int first_char_idx = first(line);

        if (first_char_idx == -1) continue;
        if (line.s[first_char_idx] == '(') continue;

        String cleaned = clean(line);

        if (startswith(cleaned, "@"))
            write_hack(variable(cleaned));
        else
            write_hack(instruction_parse(cleaned));
    }
}


void init_tables()
{
    DEST = dest();
    JUMP = jump();
    COMP_0 = comp_zero();
    COMP_1 = comp_one();
    SYMBOLS = symbols();
}


int main()
{
    // init symbol tables    
    init_tables();

    Array *arr = readlines("t.asm");
    assemble(arr);
    return 0;
}
