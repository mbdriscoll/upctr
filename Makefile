FILES=upctr.C UpcLibrary.C
HEADERS=UpcLibrary.h
CXXFLAGS=-I/usr/local/include
LDFLAGS=-L/usr/local/lib -lrose

all: upctr

upctr: upctr.o UpcLibrary.o

check: upctr
	./upctr tests/test01.upc -rose:upc_threads 4 -rose:skipFinalCompileStep
	
clean:
	rm -rf *.o upctr
