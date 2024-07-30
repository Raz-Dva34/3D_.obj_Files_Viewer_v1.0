CC=gcc -std=c11 -Wall -Werror -Wextra -g -pedantic
GCOVFLAGS =  -fprofile-arcs -ftest-coverage 
OS = $(shell uname)
BUILD_DIR = build
BUILDER = cmake
RM = rm -rf
ifeq ($(OS), Linux)
#-lsubunit
	F_LIB_CHECK=-lpthread -lcheck -pthread -lrt -lm -lsubunit
else
	F_LIB_CHECK=-lcheck -lm -lpthread 
endif

all: gcov_report install dvi

install:
	mkdir $(BUILD_DIR)
	cd $(BUILD_DIR) && $(BUILDER) ../3D_Viewer && $(MAKE)

uninstall:
	$(RM) $(BUILD_DIR)

dvi:
	mkdir -p manual
	makeinfo manual.texi --html
	open ./manual/index.html

dvi_clean:
	rm -rf manual

dist:
	tar -czvf 3D_Viewer.tar.gz *

test:
	$(CC) $(GCOVFLAGS) tests/*.c -o s21_test $(F_LIB_CHECK) -lgcov --coverage
	./s21_test

gcov_report: test
	lcov -t "test" -o report.info -c -d . 
	genhtml -o gcov_report/ report.info
	rm -f *.gcno *.gcda *.info report.out *.gcov
	open ./gcov_report/index.html

clean: uninstall
	@$(RM) *.o \
		*.a \
		*.app \
		*.so \
		*.gcno \
		*gcda \
		*.gcov \
		gcov_report \
		*.dSYM \
        RESULT_VALGRIND.txt \
		*.out \
        s21_test \
        vtests_run \
		run \
		*.gz \
		manual \
		.clang-format

style:
	cp ../materials/linters/.clang-format .
	@echo "======= CHECK GOOGLE CODE STYLE ======="
	clang-format -style=Google -n backend/*.c backend/*.h 3D_Viewer/*.cpp 3D_Viewer/*.h tests/*.c tests/*.h

valgrind:
	cd ../materials/build/Valgrind && cd $(@D) && bash run.sh

idocker:
	rm -rf ~/Library/Containers/com.docker.docker && mkdir -p ~/goinfre/Docker/Data && ln -s ~/goinfre/Docker ~/Library/Containers/com.docker.docker
