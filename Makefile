default: check

SHELL=/bin/bash

CFLAGS += -g -Wall -Wextra -pedantic -Werror

ifdef DEBUG
MAKEFILE_DEBUG=$(DEBUG)
else
MAKEFILE_DEBUG=0
endif
ifeq ($(MAKEFILE_DEBUG),0)
CFLAGS += -DNDEBUG -O2 -Wno-unused-parameter
else
CFLAGS += -DDEBUG -O0
endif

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

test-out-of-memory: src/easy-alloc.h src/easy-alloc.c \
			demo/foo.h demo/foo.c demo/test-out-of-memory.c
	$(CC) $(CFLAGS) -Isrc/ -Idemo/ \
		src/easy-alloc.c \
		demo/foo.c \
		demo/test-out-of-memory.c \
		-o test-out-of-memory

clean:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd demo && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd

tidy:
	$(LINDENT) \
		-T FILE -T size_t -T ssize_t -T bool \
		-T int8_t -T int16_t -T int32_t -T int64_t \
		-T uint8_t -T uint16_t -T uint32_t -T uint64_t \
		demo/*.c demo/*.h \
		src/*.c src/*.h

check: test-out-of-memory
	./test-out-of-memory