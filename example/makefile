CC=gcc
INCLUDE_DIR=$(HOME)/usr/include
LIB_DIR=$(HOME)/usr/lib
CFLAGS=-Wall -O3 -funroll-loops -std=c99 -I$(INCLUDE_DIR) -L$(LIB_DIR)

LINK=-lplotinc -lcairo -lX11 -lm

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test *.svg
