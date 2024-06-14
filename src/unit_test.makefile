CFLAGS= -Wall -Wextra -Werror -std=c11
LIBS=-pthread -lcheck_pic -pthread -lrt -lm -lsubunit
GCOVFLAGS = -fprofile-arcs -ftest-coverage -fPIC -O0

all: unit_test gcov_report

unit_test: s21_unit_test.o s21_decimal.o convertors.o arithmetic.o
	gcc -o unit_test s21_unit_test.o s21_decimal.o arithmetic.o $(LIBS)
	

s21_unit_test.o: s21_unit_test.c s21_decimal.h
	gcc $(CFLAGS) -c s21_unit_test.c -lm

s21_decimal.o:
	gcc $(CFLAGS) -c s21_decimal.c -lm

convertors.o:
	gcc $(CFLAGS) -c convertors.c -lm

arithmetic.o:
	gcc $(CFLAGS) -c arithmetic.c -lm

clean:
	rm -rf *.o unit_test gcov_report.*

gcov_report: s21_unit_test.o s21_decimal.o convertors.o arithmetic.o
	gcc $(GCOVFLAGS) s21_unit_test.c s21_decimal.c convertors.c arithmetic.c -o "gcov_report.out" $(LIBS)
	./gcov_report.out
	gcov -f *.gcda
	pip install gcovr
	~/.local/bin/gcovr -r . --exclude-unreachable-branches --html-details -o gcov_report.html 
	firefox gcov_report.html
	rm -f *.gcno *.gcda *.c.gcov 

