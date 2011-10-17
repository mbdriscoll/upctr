FILES=upctr.C UpcLibrary.C
HEADERS=UpcLibrary.h
CXXFLAGS=-I/usr/local/include
LDFLAGS=-L/usr/local/lib -lrose

all: upctr

upctr: upctr.o UpcLibrary.o

clean:
	rm -rf *.o upctr
