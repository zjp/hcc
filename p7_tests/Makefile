TESTFILES := $(wildcard *.holeyc)
TESTS := $(TESTFILES:.holeyc=.test)
LIBLINUX := -dynamic-linker /lib64/ld-linux-x86-64.so.2

.PHONY: all

all: $(TESTS)

%.test:
	@rm -f $*.err $*.3ac $*.s
	@touch $*.err $*.3ac $*.s
	@echo "TEST $*"
	@../holeycc $*.holeyc -o $*.s ;\
	COMP_EXIT_CODE=$$?;
	@as -o $*.o
	@ld -o $*.prog $(LIBLINUX) -lc ../entry.o ../stdholeyc.o $*.o
	objdump -Dwrt $
	./$*.out < $*.in > $*.out; \
	diff -B --ignore-all-space $*.out $*.out.expected;\
	RUN_DIFF_EXIT=$$?;\
	exit $$RUN_DIFF_EXIT

clean:
	rm -f *.3ac *.out *.err *.o *.s
