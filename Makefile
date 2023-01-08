.PHONY: all build clean test

all: build test

include config.mk

clean:
	rm -fr $(BIN) $(LIB) $(OBJ)

build: platform-build starfield-build starfield-client-build starfield-server-build

test: platform-test starfield-test

include platform/makefile.mk
include starfield/makefile.mk
include starfield-client/makefile.mk
include starfield-server/makefile.mk
