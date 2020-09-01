# Authors: Drew Davidson, Zachary Pearson
# Project: EECS 665 Project 1
# Date: 27 August 2020

####### BEGIN DEFINITIONS **********
BINNAME := holycc
SRCDIR := ./src
INCDIR := ./inc
OBJDIR := ./obj
TESTDIR := ./tests

LEXER_TOOL := flex

HOST_SYS := $(shell uname -s)

CPP_SRCS := $(wildcard $(SRCDIR)/*.cpp)

OBJ_SRCS := $(OBJDIR)/lexer.o $(CPP_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TESTS := $(wildcard $(TESTDIR)/*.holyc)

DEPS := $(OBJ_SRCS:.o=.d)

.PHONY: all pre-build rebuild clean test lsp-refs cleantest

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

# Generate compile_commands.json, a file that can be parsed by C++ clients to
# LSP. Enables jump-to-definition and in-window documentation in Emacs, among
# other things.
lsp-refs: clean
	bear make

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -g -std=c++14 -I$(INCDIR) -MMD -MP -c -o $@ $<

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
	$(CXX) -g -std=c++14 -I$(INCDIR) -c $(SRCDIR)/lexer.yy.cc -o $(OBJDIR)/lexer.o

test: 
	for file in $(TESTS); \
	do \
		./holycc $$file -t $${file%.*}.out; \
		diff $${file%.*}.out $${file%.*}.out.expected; \
	done

cleantest:
	for file in $(TESTS); \
	do \
		rm -f $${file%.*}.out $${file%.*}.err; \
	done
