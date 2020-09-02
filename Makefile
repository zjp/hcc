# Authors: Drew Davidson, Zachary Pearson
# Project: EECS 665 Project 1
# Date: 27 August 2020

####### BEGIN DEFINITIONS **********
BINNAME := holycc
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

OBJ_SRCS := $(OBJDIR)/lexer.o $(CPP_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TESTS := $(wildcard $(TESTDIR)/*.holyc)

DEPS := $(OBJ_SRCS:.o=.d)

ifeq ($(HOST_SYS),Darwin)
	ifneq (,$(shell stat /usr/local/opt/flex/include))
		INCLUDES = /usr/local/opt/flex/include
	endif
endif

ifndef INCLUDES
	INCLUDES = /usr/include/
endif

# On macOS, g++ is symlinked to clang (so is c++). When g++ is installed
# via homebrew, homebrew appends the version to the binary so that it is
# not in conflict with the symlink.

.PHONY: all pre-build rebuild retest clean test lsp-refs cleantest

####### END DEFINITIONS **********
all:
	@ echo "Compiling for $(HOST_SYS)"
	make holycc

clean:
	rm -rf $(OBJDIR) $(SRCDIR)/*.cc $(DEPS) $(BINNAME)

-include $(DEPS)

holycc: pre-build $(OBJ_SRCS)
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

$(SRCDIR)/lexer.yy.cc: $(SRCDIR)/holyc.l
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

test: 
	@ echo ""
	for file in $(TESTS); \
	do \
		echo ""; \
		echo $$file; \
		./holycc $$file -t $${file%.*}.out 2> $${file%.*}.err; \
		echo "Diff of output"; \
		diff --text $${file%.*}.out $${file%.*}.out.expected; \
		echo "Diff of error"; \
		diff --text $${file%.*}.err $${file%.*}.err.expected; \
	done

cleantest:
	for file in $(TESTS); \
	do \
		rm -f $${file%.*}.out $${file%.*}.err; \
	done
