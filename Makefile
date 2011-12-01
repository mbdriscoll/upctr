FILES=upctr.C UpcLibrary.C
HEADERS=UpcLibrary.h
MBD_CXXFLAGS=-I/Users/driscoll6/rose-clean/install/include -I/Users/driscoll6/macports/include
MBD_LDFLAGS=-L/Users/driscoll6/rose-clean/install/lib -L/Users/driscoll6/macports/lib
CXXFLAGS=-I/usr/local/include $(MBD_CXXFLAGS)
LDFLAGS=-L/usr/local/lib $(MBD_LDFLAGS) -lrose -lstdc++
UPCC=upc

all: upctr

upctr: upctr.o UpcLibrary.o

check: upctr
	./upctr tests/test01.upc -rose:upc_threads 4 -rose:skipFinalCompileStep

dgemm:
	$(UPCC) -I./tests -g -Wall -o dgemm tests/test03.upc

benchmark: dgemm
	time ./dgemm -n 8

clean:
	rm -rf *.o upctr *.upc
