# Makefile by Michal Trojnara 2016

CFLAGS=-g -O3 -Wall -I ../src
LDLIBS=-lcrypto
EXE=performance benchmark test prng streamhash4sum

all: $(EXE)

$(EXE): ../src/streamhash4.o

../src/streamhash4.o:
	$(MAKE) -C ../src

clean:
	rm -f $(EXE) *.o

dieharder: prng
	./prng big | dieharder -g 200 -a

ent: prng
	./prng big | dd bs=1k count=100k status=noxfer | ent

performance: performance.o

benchmark: benchmark.o

test: test.o

prng: prng.o

streamhash4sum: streamhash4sum.o

