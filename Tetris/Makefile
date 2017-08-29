# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive
LDFLAGS_TET=
LDFLAGS_CHAT=-lpthread
DEPS_TET=CTetris.h Tetris.h Matrix.h
DEPS_CHAT=bool.h colors.h log.h llist2.h
OBJS_TET=btetcli.o CTetris.o Tetris.o Matrix.o
OBJS_CHAT=chatsrv.o llist2.o log.o
DEBUG=0

all:: chatsrv.exe btetcli.exe

btetcli.exe: $(OBJS_TET)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS_TET)
	cp btetcli.exe btetclisolo.exe
	cp btetcli.exe btetclisend.exe
	cp btetcli.exe btetclirecv.exe

chatsrv.exe: $(OBJS_CHAT)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS_CHAT)

%.o: %.c $(DEPS_CHAT)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS_TET)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f chatsrv.exe btetcli.exe btetclisolo.exe btetclisend.exe btetclirecv.exe
	rm -f *.o *~ *.stackdump
