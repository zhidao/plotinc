CC=gcc
CFLAGS=-Wall -fPIC -fno-common -O3 -funroll-loops -std=c99 -I../include

LD=$(CC)
LDFLAGS=-shared

TARGET=libplotinc.so
OBJ=plotinc.o

all: $(TARGET)
$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f $(OBJ) *~ $(TARGET)
