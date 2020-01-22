
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), FreeBSD)
	CC=cc
else
	CC=gcc
endif

ifeq ($(UNAME_S), Darwin)
	LIBNAME=libmfoundation.dylib
else
	LIBNAME=libmfoundation.so
endif

CFLAGS= -Wall -std=c99 -Wdeprecated-declarations -DM_FOUNDATION_IMPORT_ALL

DEBUG= -g
RELEASE= -O2

LIBS= -lc
TLIBS= $(LIBS) -Lbuild -lmfoundation

LIB_SRCS := $(shell find src -name "*.c")
APP_SRCS := $(shell find unit_test -name "*.c")

DIRS := $(shell find src -type d)

INCS := -I. $(foreach n, $(DIRS), -I$(n))

all: debug

debug: $(LIB_SRCS)
	mkdir -p build
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/$(LIBNAME) $^ $(LIBS) -shared -fPIC

release: $(LIB_SRCS)
	mkdir -p build
	$(CC) $(RELEASE) $(CFLAGS) $(INCS) -o build/$(LIBNAME) $^ $(LIBS) -shared -fPIC

test: $(APP_SRCS)
	mkdir -p build
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_timer.out $^ $(TLIBS) -DM_FOUNDATION_TEST_TIMER
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_skiplist.out $^ $(TLIBS) -DM_FOUNDATION_TEST_SKIPLIST
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_debug.out $^ $(TLIBS) -DM_FOUNDATION_TEST_DEBUG
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_sha256.out $^ $(TLIBS) -DM_FOUNDATION_TEST_SHA256
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_chacha20.out $^ $(TLIBS) -DM_FOUNDATION_TEST_CHACHA20

clean:
	rm -rf build
