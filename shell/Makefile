CXX = g++
CXXFLAGS = -Wall -Werror -pedantic -Og -g -std=c++17 -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG

all: msh

msh: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o msh

test: msh
	python3 shell_test.py

SUBMIT_FILENAME=ap4gf.lvp2dun.shell1.tar.gz

archive:
	tar "--exclude=*.tgz" --exclude=msh --exclude=.git "--exclude=*.o" -zcvf $(SUBMIT_FILENAME) *
	@echo "Created $(SUBMIT_FILENAME); please upload and submit this file."

submit: archive

.PHONY: clean all test archive submit
