FILES=upctr.C UpcLibrary.C optimize.C
HEADERS=UpcLibrary.h optimize.h
MBD_CXXFLAGS=-I/Users/driscoll6/rose-clean/install/include -I/Users/driscoll6/macports/include
MBD_LDFLAGS=-L/Users/driscoll6/rose-clean/install/lib -L/Users/driscoll6/macports/lib
CXXFLAGS=-I/usr/local/include $(MBD_CXXFLAGS) -g -Wall
LDFLAGS=-L/usr/local/lib $(MBD_LDFLAGS) -lrose -lstdc++
UPCC=upcc

all: upctr

upctr: upctr.o UpcLibrary.o optimize.o

check: upctr
	./upctr tests/dgemm.upc -debug -Itests -rose:upc_threads 4 -rose:skipFinalCompileStep

clean:
	rm -rf *.o upctr *.upc
