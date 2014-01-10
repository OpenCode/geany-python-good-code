OS := $(shell uname)
ARCH := $(shell uname -m)
CC=gcc
NAME=python-good-code

LIBS:= -fPIC `pkg-config --cflags geany`
SHAREDLIBS:= -shared `pkg-config --libs geany`
SOURCES:=src/$(NAME).c
OBJ=$(SOURCES:.c=.o)
SLOBJ=$(SOURCES:.c=.so)
SOBJ=$(NAME).so

ifdef GEANY_PATH
   GPATH=$(GEANY_PATH)
else
   GPATH=/usr/lib/$(ARCH)-linux-gnu/geany
endif

init:
	$(CC) -c $(SOURCES) $(LIBS) -o $(OBJ)
	$(CC) $(OBJ) -o  $(SLOBJ)  $(SHAREDLIBS)

install:
	cp $(SLOBJ) $(GPATH)/$(SOBJ)


clean:
	rm -rf $(OBJ)
	rm -rf $(SOBJ)
	rm  $(GPATH)/$(SOBJ

