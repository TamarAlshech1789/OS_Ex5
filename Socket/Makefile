CC=g++
RANLIB=ranlib

LIBSRC=sockets.cpp


CCFLAGS = -Wall -std=c++11 -g

TARGETS = sockets

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS)
	$(CC) $(TARGETS).o -o $(TARGETS)

$(TARGETS): sockets.cpp
	$(CC) $(CCFLAGS) -c sockets.cpp

clean:
	$(RM) $(TARGETS) $(MAPREDUCEFRAMEWORKLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CCFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)