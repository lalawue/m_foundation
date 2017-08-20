
CC=gcc
CFLAGS= -Wall -std=c99 -Wdeprecated-declarations

DEBUG= -g
RELEASE= -O2

LIBS= -lc

LIB_SRCS := $(shell find . -name "*.c")

DIRS := $(shell find . -type d)

INCS := $(foreach n, $(DIRS), -I$(n))

all: debug

debug: $(LIB_SRCS)
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o libmfoundation.dylib $^ $(LIBS) -shared

release: $(LIB_SRCS)
	$(CC) $(RELEASE) $(CFLAGS) $(INCS) -o libmfoundation.out $^ $(LIBS) -shared

clean:
	rm -rf *.dylib *.dSYM
