# Newspost 1.13

# makefile for newspost

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
CFLAGS = -O
CC = cc
INSTALL = install
SRC_FILES = uuencode.c newspost.c uu.c nntp.c socket.c utils.c cksfv/crc32.c cksfv/newsfv.c cksfv/print.c

all: newspost

newspost: $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES) 

install: newspost
	$(INSTALL) -c -m 755 newspost $(BINDIR)

clean:
	-cd cksfv ; rm -f *~ *.o ; cd .. ; 
	-rm -f *~ *.o newspost

