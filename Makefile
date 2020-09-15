# Authors: Drew Davidson, Zachary Pearson
# Project: EECS 665 Project 1
# Date: 27 August 2020

####### BEGIN DEFINITIONS **********
BINNAME := holeycc
SRCDIR := ./src
INCDIR := ./inc
OBJDIR := ./obj
TESTDIR := ./tests

HOST_SYS := $(shell uname -s)

ifeq ($(HOST_SYS),Darwin)
	ifneq (,$(shell stat /usr/local/Cellar/flex/2.6.4_1/bin/flex))
		LEXER_TOOL := /usr/local/Cellar/flex/2.6.4_1/bin/flex
	endif
endif

ifndef LEXER_TOOL
	LEXER_TOOL := flex
endif

CPP_SRCS := $(wildcard $(SRCDIR)/*.cpp)

OBJ_SRCS := $(OBJDIR)/parser.o $(OBJDIR)/lexer.o $(CPP_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

LEXER_TESTS := $(wildcard $(TESTDIR)/lexer/*.holeyc)
PARSER_TESTS := $(wildcard $(TESTDIR)/parser/*.holeyc)

DEPS := $(OBJ_SRCS:.o=.d)

FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy\
	  -Wdisabled-optimization -Wformat=2 -Wuninitialized -Winit-self\
	  -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast\
	  -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo\
	  -Wstrict-overflow=5 -Wundef -Werror -Wno-unused -Wno-unused-parameter

# Grab the right version of flex
ifeq ($(HOST_SYS),Darwin)
	ifneq (,$(shell stat /usr/local/opt/flex/include))
		INCLUDES = /usr/local/opt/flex/include
	endif
endif

ifndef INCLUDES
	INCLUDES = /usr/include/
endif

# Grab the right version of bison
ifeq ($(HOST_SYS),Darwin)
	ifneq (,$(shell stat /usr/local/opt/bison/bin/bison))
		BISON = /usr/local/opt/bison/bin/bison
	endif
endif

ifndef BISON
	BISON = bison
endif

# On macOS, g++ is symlinked to clang (so is c++). When g++ is installed
# via homebrew, homebrew appends the version to the binary so that it is
# not in conflict with the symlink.

.PHONY: all pre-build rebuild retest clean lsp-refs\
	test test-lexer test-parser cleantest cleanalltest\
	clean-parser-test clean-lexer-test

####### END DEFINITIONS **********
all:
	@ echo "Compiling for $(HOST_SYS)"
	make holeycc

clean:
	rm -rf *.output $(OBJDIR) $(INCDIR)/*.hh $(SRCDIR)/*.cc $(SRCDIR)/*.hh $(DEPS) $(BINNAME)

-include $(DEPS)

holeycc: pre-build $(OBJ_SRCS)
	$(CXX) -g -std=c++14 -I$(INCDIR) -o $@ $(OBJ_SRCS)

pre-build:
	@ echo "Attempting to make object directory..."
	- mkdir obj

rebuild: clean all

retest: clean all test

# Generate compile_commands.json, a file that can be parsed by C++ clients to
# LSP. Enables jump-to-definition and in-window documentation in Emacs, among
# other things.
lsp-refs: clean
	bear make

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -g -std=c++14 -I$(INCDIR) -I$(INCLUDES) -MMD -MP -c -o $@ $<


$(OBJDIR)/parser.o: $(SRCDIR)/parser.cc
	$(CXX) $(FLAGS) -I$(INCDIR) -I$(INCLUDES) -Wno-sign-compare -Wno-sign-conversion -Wno-switch-default -g -std=c++14 -MMD -MP -c -o $@ $<

$(SRCDIR)/parser.cc: $(SRCDIR)/holeyc.yy
	$(BISON) -Werror -Wcounterexamples --defines=$(INCDIR)/grammar.hh -v $<
	mv parser.cc $(SRCDIR)
# This file is useless as of Bison 3.2. Cycle servers have 3.4.1 -- Locally, I have 3.7.1
	- rm stack.hh

$(SRCDIR)/lexer.yy.cc: $(SRCDIR)/holeyc.l
	$(LEXER_TOOL) --outfile=$(SRCDIR)/lexer.yy.cc $<

# In case we're using an old version of flex, and instead of suppressing warnings
# globally, let's just cut the 'register' keyword out of flex's output. 
$(OBJDIR)/lexer.o: $(SRCDIR)/lexer.yy.cc
ifeq ($(HOST_SYS),Darwin)
	sed -i "" -e 's/register//g' $(SRCDIR)/lexer.yy.cc
else
	sed -i"" -e 's/register//g' $(SRCDIR)/lexer.yy.cc
endif
	$(CXX) -g -std=c++14 -I$(INCDIR) -I$(INCLUDES) -c $(SRCDIR)/lexer.yy.cc -o $(OBJDIR)/lexer.o

test: test-parser

testall: test-lexer test-parser 

test-parser:
	@ echo ""
	for file in $(PARSER_TESTS); \
	do \
		echo ""; \
		echo $$file; \
		./holeycc $$file -p 1> /dev/null 2> $${file%.*}.err; \
		echo "Diff of error"; \
		diff --text $${file%.*}.err $${file%.*}.err.expected; \
	done

test-lexer: 
	@ echo ""
	for file in $(LEXER_TESTS); \
	do \
		echo ""; \
		echo $$file; \
		./holeycc $$file -t $${file%.*}.out 2> $${file%.*}.err; \
		echo "Diff of output"; \
		diff --text $${file%.*}.out $${file%.*}.out.expected; \
		echo "Diff of error"; \
		diff --text $${file%.*}.err $${file%.*}.err.expected; \
	done

cleantest: clean-parser-test

clean-parser-test:
	for file in $(PARSER_TESTS); \
	do \
		rm -f $${file%.*}.err; \
	done

clean-lexer-test:
	for file in $(LEXER_TESTS); \
	do \
		rm -f $${file%.*}.out $${file%.*}.err; \
	done

cleanalltest: clean-parser-test clean-lexer-test
