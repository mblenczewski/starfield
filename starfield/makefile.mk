.PHONY: starfield starfield-build starfield-test

STARFIELD_MAJOR		:= 0
STARFIELD_MINOR		:= 1
STARFIELD_PATCH		:= 0
STARFIELD_VERSION	:= $(STARFIELD_MAJOR).$(STARFIELD_MINOR).$(STARFIELD_PATCH)

STARFIELD_CFLAGS	:= \
			   $(CFLAGS) \
			   $(CPPFLAGS) \
			   -DSTARFIELD_VERSION_MAJOR="\"$(STARFIELD_MAJOR)"\" \
			   -DSTARFIELD_VERSION_MINOR="\"$(STARFIELD_MINOR)"\" \
			   -DSTARFIELD_VERSION_PATCH="\"$(STARFIELD_PATCH)"\" \
			   -DSTARFIELD_VERSION="\"$(STARFIELD_VERSION)"\" \
			   -Iplatform/include \
			   -Istarfield/include

STARFIELD_FLAGS	:= \
			   $(STARFIELD_CFLAGS) \
			   $(LDFLAGS) \
			   -lplatform

STARFIELD_SOURCES	:= starfield/src/starfield.c
STARFIELD_OBJECTS	:= $(STARFIELD_SOURCES:%.c=$(OBJ)/%.c.o)
STARFIELD_OBJDEPS	:= $(STARFIELD_OBJECTS:%.o=%.d)

-include $(STARFIELD_OBJDEPS)

STARFIELD_TEST_SOURCES	:= starfield/tests/test_starfield.c
STARFIELD_TEST_OBJECTS	:= $(STARFIELD_TEST_SOURCES:%.c=$(TST)/%.x)

$(STARFIELD_OBJECTS): $(OBJ)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -o $@ -c $< $(STARFIELD_CFLAGS)

$(STARFIELD_TEST_OBJECTS): $(TST)/%.x: starfield-test-deps %.c
	@mkdir -p $(dir $@)
	$(CC) -static -o $@ $(word 2,$^) $(STARFIELD_FLAGS) -lstarfield

$(LIB)/libstarfield.$(STARFIELD_VERSION).a: starfield-deps $(STARFIELD_OBJECTS) | $(LIB)
	$(AR) -rcs $@ $(wordlist 2,$(words $^),$^)

$(LIB)/libstarfield.$(STARFIELD_MAJOR).a: $(LIB)/libstarfield.$(STARFIELD_VERSION).a
	ln -sf $(notdir $<) $@

$(LIB)/libstarfield.a: $(LIB)/libstarfield.$(STARFIELD_MAJOR).a
	ln -sf $(notdir $<) $@

starfield-deps: $(LIB)/libplatform.a

starfield-build: $(LIB)/libstarfield.a

starfield-test-deps: $(LIB)/libplatform.a $(LIB)/libstarfield.a

starfield-test: $(STARFIELD_TEST_OBJECTS)
	@for f in $(STARFIELD_TEST_OBJECTS); do ./$$f ; done

starfield: starfield-build starfield-test
