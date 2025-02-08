PROJNAME=plotinc

CC=gcc
HEADER_DIR=../include
CFLAGS=-Wall -fPIC -fno-common -O3 -funroll-loops -std=c99 -I${HEADER_DIR}

LD=$(CC)
LDFLAGS=-shared

TARGET=libplotinc.so
OBJ=plotinc.o

all: $(TARGET)
$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^
%.o: %.c ${HEADER_DIR}/${PROJNAME}/%.h
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f $(OBJ) *~ $(TARGET)
