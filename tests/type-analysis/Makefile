TESTFILES := $(wildcard *.holeyc)
TESTS := $(TESTFILES:.holeyc=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@echo "Testing $*.holeyc"
	@touch $*.out #Creates out file to diff in case of no output
	@touch $*.err #The @ means don't show the command being invoked
	@../holeycc $*.holeyc -c 2> $*.err ;\
	PROG_EXIT_CODE=$$?;\
	echo "diff error...";\
	diff $*.err $*.err.expected;\
	ERR_EXIT_CODE=$$?;\
	exit $$ERR_EXIT_CODE

clean:
	rm *.out *.err
