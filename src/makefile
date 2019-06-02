
CC = gcc
CFLAGS = -std=c99 -O2 -Wall -w
POSIXTHREADS = -lpthread
OBJS = main.o gen.o common.o thread.o tm.o notation.o search.o eval.o xboard.o hash.o move.o tt.o variants.o debug.o book.o
OUT=../bin/nino
SOURCES = main.c gen.c common.c thread.c tm.c notation.c search.c eval.c xboard.c hash.c move.c tt.c variants.c debug.c book.c 
RM = rm

default: nino clean

nino: $(OBJS) 
	@$(CC) $(CFLAGS) -o $(OUT) $(OBJS) $(POSIXTHREADS) 

win32: 
	i686-w64-mingw32-gcc -o $(OUT)32.exe $(CFLAGS) $(SOURCES)

win64: 
	x86_64-w64-mingw32-gcc -o $(OUT)64.exe $(CFLAGS) $(SOURCES)

clean: 
	@$(RM) *.o

all: nino win32 win64 clean

install: 
	@sudo cp $(OUT)/* /usr/bin
