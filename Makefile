CC=gcc
CFLAGS=-std=c11 -Werror -Wall -Wconversion
LDLIBS=-lm
CPPCHECK=cppcheck --enable=performance,unusedFunction --error-exitcode=1 *.c
OUTPUT_TARGET=main
OUTPUT_MODULES=bmp.o transformations.o main.o
TEST_TARGET=tests/all_tests
TEST_MODULES=tests/bmp_tests.o

all:$(OUTPUT_TARGET)

$(OUTPUT_TARGET): $(OUTPUT_MODULES)
	$(CPPCHECK)
	$(CC) $(CFLAGS) $(OUTPUT_MODULES) $(LDLIBS) -o $(OUTPUT_TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c $^ $(LDLIBS)

bmp: bmp.o
	$(CPPCHECK)
	$(CC) $(CFLAGS) bmp.o $(LDLIBS) -o bmp

bmp.o: bmp.c bmp.h
	$(CC) $(CFLAGS) -c $^ $(LDLIBS)

transformations: transformations.o
	$(CPPCHECK)
	$(CC) $(CFLAGS) transformations.o $(LDLIBS) -o transformations

transformations.o: transformations.c transformations.h
	$(CC) $(CFLAGS) -c $^ $(LDLIBS)

test: $(TEST_TARGET)
	$<

$(TEST_TARGET): tests/all_tests.o $(TEST_MODULES) $(MODULES)

all_tests: tests/all_tests.o $(TEST_MODULES) $(OUTPUT_MODULES)

tests/test_bmp.o: tests/bmp_tests.c bmp.h

clean:
	rm -rf $(OUTPUT) *.o

