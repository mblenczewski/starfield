.PHONY: platform platform-build platform-test

PLATFORM_MAJOR		:= 0
PLATFORM_MINOR		:= 1
PLATFORM_PATCH		:= 0
PLATFORM_VERSION	:= $(PLATFORM_MAJOR).$(PLATFORM_MINOR).$(PLATFORM_PATCH)

PLATFORM_CFLAGS		:= \
			   $(CFLAGS) \
			   $(CPPFLAGS) \
			   -DPLATFORM_VERSION_MAJOR="\"$(PLATFORM_MAJOR)"\" \
			   -DPLATFORM_VERSION_MINOR="\"$(PLATFORM_MINOR)"\" \
			   -DPLATFORM_VERSION_PATCH="\"$(PLATFORM_PATCH)"\" \
			   -DPLATFORM_VERSION="\"$(PLATFORM_VERSION)"\" \
			   -Iplatform/include

PLATFORM_FLAGS		:= \
			   $(PLATFORM_CFLAGS) \
			   $(LDFLAGS)

PLATFORM_SOURCES	:= platform/src/platform.c
PLATFORM_OBJECTS	:= $(PLATFORM_SOURCES:%.c=$(OBJ)/%.c.o)
PLATFORM_OBJDEPS	:= $(PLATFORM_OBJECTS:%.o=%.d)

-include $(PLATFORM_OBJDEPS)

PLATFORM_TEST_SOURCES	:= platform/tests/test_platform.c
PLATFORM_TEST_OBJECTS	:= $(PLATFORM_TEST_SOURCES:%.c=$(TST)/%.x)

$(PLATFORM_OBJECTS): $(OBJ)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -o $@ -c $< $(PLATFORM_CFLAGS)

$(PLATFORM_TEST_OBJECTS): $(TST)/%.x: platform-test-deps %.c
	@mkdir -p $(dir $@)
	$(CC) -static -o $@ $(word 2,$^) $(PLATFORM_FLAGS) -lplatform

$(LIB)/libplatform.$(PLATFORM_VERSION).a: platform-deps $(PLATFORM_OBJECTS) | $(LIB)
	$(AR) -rcs $@ $(wordlist 2,$(words $^),$^)

$(LIB)/libplatform.$(PLATFORM_MAJOR).a: $(LIB)/libplatform.$(PLATFORM_VERSION).a
	ln -sf $(notdir $<) $@

$(LIB)/libplatform.a: $(LIB)/libplatform.$(PLATFORM_MAJOR).a
	ln -sf $(notdir $<) $@

platform-deps:

platform-build: $(LIB)/libplatform.a

platform-test-deps: $(LIB)/libplatform.a

platform-test: $(PLATFORM_TEST_OBJECTS)
	@for f in $(PLATFORM_TEST_OBJECTS); do ./$$f ; done

platform: platform-build platform-test
