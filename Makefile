default: check

SHELL=/bin/bash
BROWSER=firefox

CFLAGS += -g -Wall -Wextra -pedantic -Werror

ifdef DEBUG
MAKEFILE_DEBUG=$(DEBUG)
else
MAKEFILE_DEBUG=0
endif
ifeq ($(MAKEFILE_DEBUG),0)
CFLAGS += -DNDEBUG -O2
else
CFLAGS += -DDEBUG -O0 \
	-fno-inline-small-functions \
	-fkeep-inline-functions \
	-fkeep-static-functions
endif

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

test-out-of-memory: demo/test-out-of-memory.c \
			src/context-alloc.h \
			src/context-alloc.c \
			util/oom-injecting-malloc.h \
			util/oom-injecting-malloc.c \
			demo/malloc-whine.h \
			demo/foo.h \
			demo/foo.c
	$(CC) $(CFLAGS) \
		-I./src -I./util -I./demo \
		src/context-alloc.c \
		util/oom-injecting-malloc.c \
		demo/foo.c \
		demo/test-out-of-memory.c \
		-o test-out-of-memory \
		--coverage

line-cov: check
	lcov --capture --directory . --output-file coverage.info

html-report: line-cov
	genhtml coverage.info --output-directory coverage_html

coverage: html-report
	$(BROWSER) ./coverage_html/demo/foo.c.gcov.html

clean:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd demo && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd

tidy:
	$(LINDENT) \
		-T FILE -T size_t -T ssize_t -T bool \
		-T int8_t -T int16_t -T int32_t -T int64_t \
		-T uint8_t -T uint16_t -T uint32_t -T uint64_t \
		-T oom_injecting_context_s \
		-T foo_s -T foo_list_s \
		demo/*.c demo/*.h \
		util/*.c util/*.h \
		src/*.c src/*.h

check: test-out-of-memory
	./test-out-of-memory
