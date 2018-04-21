
CC=gcc
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
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/libmfoundation.dylib $^ $(LIBS) -shared -fPIC

release: $(LIB_SRCS)
	mkdir -p build
	$(CC) $(RELEASE) $(CFLAGS) $(INCS) -o build/libmfoundation.dylib $^ $(LIBS) -shared -fPIC

test: $(APP_SRCS)
	mkdir -p build
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_timer.out $^ $(TLIBS) -DM_FOUNDATION_TEST_TIMER
	$(CC) $(DEBUG) $(CFLAGS) $(INCS) -o build/test_skiplist.out $^ $(TLIBS) -DM_FOUNDATION_TEST_SKIPLIST

clean:
	rm -rf build
