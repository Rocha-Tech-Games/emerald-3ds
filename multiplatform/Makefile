CC ?= cc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Werror
CPPFLAGS += -Iinclude
.PHONY: all verify clean
all: build/world-tool
build:
	mkdir -p build
build/world-tool: src/world.c host/main.c include/world.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/world.c host/main.c -o $@
build/test-world: src/world.c tests/test_world.c include/world.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) src/world.c tests/test_world.c -o $@
verify: all build/test-world
	./build/test-world
	./build/world-tool
	python3 tests/test_cli.py
clean:
	rm -rf build
