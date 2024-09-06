	
string: 
	$(CC) \
		mystring.c \
		-o s.out \
		-Wall \
		-Wextra \
		-pedantic \
		-std=c99


array: 
	$(CC) \
		-c array.c \
		-o a.out \
		-Wall \
		-Wextra \
		-Wfloat-equal \
		-pedantic \
		-std=c99


hash:
	$(CC) \
		hash.c \
		-o h.out \
		-Wall \
		-Wextra \
		-Wfloat-equal \
		-pedantic \
		-std=c99


lib:
	$(CC) \
		-c mystring.c \
		-c array.c \
		-c hash.c \
		-Wall \
		-Wextra \
		-Wfloat-equal \
		-pedantic \
		-std=c99


asm: lib
	$(CC) \
		asm.c \
		mystring.o \
		array.o \
		hash.o \
		-o asm.out \
		-Wall \
		-Wextra \
		-Wfloat-equal \
		-pedantic \
		-std=c99


tests: test-string

# remember to compile _all_ source files needed: https://stackoverflow.com/a/29152910
test-string:
	$(CC) \
		mystring.c \
		tests.c \
		-o b.out \
		-Wall \
		-Wextra \
		-pedantic \
		-std=c99


# link static .o files: cc test_array.c array.o -o arr.out
# to generate .o file use `-c` option
test-array:
	$(CC) \
		-g array.c \
		-g test_array.c \
		-o arr.out \
		-Wall \
		-Wextra \
		-pedantic \
		-std=c99


test-hash:
	$(CC) \
		-g hash.c \
		-g test_hash.c \
		-o hash.out \
		-Wall \
		-Wextra \
		-pedantic \
		-std=c99


debug:
	$(CC) -g array.c test_array.c \
		-std=c99

clean:
	rm -rf *.out* \
	rm *.o \
	rm *.hack


.PHONY: clean tests asm lib
