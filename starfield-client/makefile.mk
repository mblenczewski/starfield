.PHONY: starfield-client starfield-client-build starfield-client-test

STARFIELD_CLIENT_MAJOR		:= 0
STARFIELD_CLIENT_MINOR		:= 1
STARFIELD_CLIENT_PATCH		:= 0
STARFIELD_CLIENT_VERSION	:= $(STARFIELD_CLIENT_MAJOR).$(STARFIELD_CLIENT_MINOR).$(STARFIELD_CLIENT_PATCH)

STARFIELD_CLIENT_CFLAGS		:= \
				   $(CFLAGS) \
				   $(CPPFLAGS) \
				   -DSTARFIELD_CLIENT_VERSION_MAJOR="\"$(STARFIELD_CLIENT_MAJOR)"\" \
				   -DSTARFIELD_CLIENT_VERSION_MINOR="\"$(STARFIELD_CLIENT_MINOR)"\" \
				   -DSTARFIELD_CLIENT_VERSION_PATCH="\"$(STARFIELD_CLIENT_PATCH)"\" \
				   -DSTARFIELD_CLIENT_VERSION="\"$(STARFIELD_CLIENT_VERSION)"\" \
				   -Iplatform/include \
				   -Istarfield/include \
				   -Istarfield-client/include

STARFIELD_CLIENT_FLAGS		:= \
				   $(STARFIELD_CLIENT_CFLAGS) \
				   $(LDFLAGS) \
				   -lplatform \
				   -lstarfield

STARFIELD_CLIENT_SOURCES	:= starfield-client/src/starfield-client.c
STARFIELD_CLIENT_OBJECTS	:= $(STARFIELD_CLIENT_SOURCES:%.c=$(OBJ)/%.c.o)
STARFIELD_CLIENT_OBJDEPS	:= $(STARFIELD_CLIENT_OBJECTS:%.o=%.d)

-include $(STARFIELD_CLIENT_OBJDEPS)

$(STARFIELD_CLIENT_OBJECTS): $(OBJ)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -o $@ -c $< $(STARFIELD_CLIENT_CFLAGS)

$(BIN)/starfield-client: starfield-client-deps $(STARFIELD_CLIENT_OBJECTS) | $(BIN)
	$(CC) -o $@ $(wordlist 2,$(words $^),$^) $(STARFIELD_CLIENT_FLAGS)

starfield-client-deps: $(LIB)/libplatform.a $(LIB)/libstarfield.a

starfield-client-build: $(BIN)/starfield-client

starfield-client-test-deps:

starfield-client-test:

starfield-client: starfield-client-build starfield-client-test
