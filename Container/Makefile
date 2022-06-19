CC=g++
RANLIB=ranlib

LIBSRC=container.cpp


CCFLAGS = -Wall -std=c++11 -g

TARGETS = container

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS)
	$(CC) $(TARGETS).o -o $(TARGETS)

$(TARGETS): container.cpp
	$(CC) $(CCFLAGS) -c container.cpp

clean:
	$(RM) $(TARGETS) $(MAPREDUCEFRAMEWORKLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CCFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)