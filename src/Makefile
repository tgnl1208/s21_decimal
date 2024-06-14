CFLAGS= -Wall -Wextra -Werror -std=c11
LIBS=-pthread -lcheck_pic -pthread -lrt -lm -lsubunit
GCOVFLAGS = -fprofile-arcs -ftest-coverage -fPIC -O0
VALGRIND = valgrind --tool=memcheck --leak-check=yes

all: s21_decimal.a

main: s21_decimal.c convertors.c arithmetic.c other_functions.c comparisons.c
	gcc s21_decimal.c convertors.c arithmetic.c other_functions.c comparisons.c -lm

test: unit_test.o s21_decimal.o convertors.o arithmetic.o other_functions.o comparisons.o
	gcc -o unit_test s21_unit_test.o s21_decimal.o convertors.o arithmetic.o other_functions.o comparisons.o $(LIBS)
	./unit_test

test_valgrind: unit_test.o s21_decimal.o convertors.o arithmetic.o other_functions.o comparisons.o
	gcc -o unit_test s21_unit_test.o s21_decimal.o convertors.o arithmetic.o other_functions.o comparisons.o $(LIBS)
	$(VALGRIND) ./unit_test

unit_test.o: s21_unit_test.c s21_decimal.h
	gcc $(CFLAGS) -c s21_unit_test.c

s21_decimal.a: s21_decimal.o convertors.o arithmetic.o other_functions.o comparisons.o
	ar rcs s21_decimal.a *.o
	ranlib s21_decimal.a

s21_unit_test.o: s21_unit_test.c s21_decimal.h
	gcc $(CFLAGS) -c s21_unit_test.c -lm

s21_decimal.o:
	gcc $(CFLAGS) -c s21_decimal.c -lm

convertors.o:
	gcc $(CFLAGS) -c convertors.c -lm

arithmetic.o:
	gcc $(CFLAGS) -c arithmetic.c -lm

other_functions.o:
	gcc $(CFLAGS) -c other_functions.c -lm

comparisons.o:
	gcc $(CFLAGS) -c comparisons.c -lm

clean:
	rm -rf *.o *.a unit_test gcov_report.*
	make -f unit_test.makefile clean

clang-format:
	clang-format -style=Google -n *.h
	clang-format -style=Google -n *.c
	clang-format -style=Google -i *.h
	clang-format -style=Google -i *.c


gcov_report: s21_unit_test.o s21_decimal.o convertors.o arithmetic.o other_functions.o comparisons.o
	gcc $(GCOVFLAGS) s21_unit_test.c s21_decimal.c convertors.c arithmetic.c other_functions.c comparisons.c -o "gcov_report.out" $(LIBS)
	./gcov_report.out
	gcov -f *.gcda
	#pip install gcovr
	~/.local/bin/gcovr -r . --exclude-unreachable-branches --html-details -o gcov_report.html 
	firefox gcov_report.html
	rm -f *.gcno *.gcda *.c.gcov 
