.PHONY: starfield-server starfield-server-build starfield-server-test

STARFIELD_SERVER_MAJOR		:= 0
STARFIELD_SERVER_MINOR		:= 1
STARFIELD_SERVER_PATCH		:= 0
STARFIELD_SERVER_VERSION	:= $(STARFIELD_SERVER_MAJOR).$(STARFIELD_SERVER_MINOR).$(STARFIELD_SERVER_PATCH)

STARFIELD_SERVER_CFLAGS		:= \
				   $(CFLAGS) \
				   $(CPPFLAGS) \
				   -DSTARFIELD_SERVER_VERSION_MAJOR="\"$(STARFIELD_SERVER_MAJOR)"\" \
				   -DSTARFIELD_SERVER_VERSION_MINOR="\"$(STARFIELD_SERVER_MINOR)"\" \
				   -DSTARFIELD_SERVER_VERSION_PATCH="\"$(STARFIELD_SERVER_PATCH)"\" \
				   -DSTARFIELD_SERVER_VERSION="\"$(STARFIELD_SERVER_VERSION)"\" \
				   -Iplatform/include \
				   -Istarfield/include \
				   -Istarfield-server/include

STARFIELD_SERVER_FLAGS		:= \
				   $(STARFIELD_SERVER_CFLAGS) \
				   $(LDFLAGS) \
				   -lplatform \
				   -lstarfield

STARFIELD_SERVER_SOURCES	:= starfield-server/src/starfield-server.c
STARFIELD_SERVER_OBJECTS	:= $(STARFIELD_SERVER_SOURCES:%.c=$(OBJ)/%.c.o)
STARFIELD_SERVER_OBJDEPS	:= $(STARFIELD_SERVER_OBJECTS:%.o=%.d)

-include $(STARFIELD_SERVER_OBJDEPS)

$(STARFIELD_SERVER_OBJECTS): $(OBJ)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -o $@ -c $< $(STARFIELD_SERVER_CFLAGS)

$(BIN)/starfield-server: starfield-server-deps $(STARFIELD_SERVER_OBJECTS) | $(BIN)
	$(CC) -o $@ $(wordlist 2,$(words $^),$^) $(STARFIELD_SERVER_FLAGS)

starfield-server-deps: $(LIB)/libplatform.a $(LIB)/libstarfield.a

starfield-server-build: $(BIN)/starfield-server

starfield-server-test-deps:

starfield-server-test:

starfield-server: starfield-server-build starfield-server-test
