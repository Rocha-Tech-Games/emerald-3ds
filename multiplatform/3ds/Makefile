DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM
CC := $(DEVKITARM)/bin/arm-none-eabi-gcc
ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
.PHONY: all clean
all: build/emerald-world-base.3dsx
build:
	mkdir -p build
build/emerald-world-base.elf: source/main.c ../src/world.c ../include/world.h | build
	$(CC) $(ARCH) -O2 -Wall -Wextra -Werror -I../include -I$(DEVKITPRO)/libctru/include source/main.c ../src/world.c -specs=3dsx.specs -L$(DEVKITPRO)/libctru/lib -lctru -lm -o $@
build/emerald-world-base.3dsx: build/emerald-world-base.elf
	$(DEVKITPRO)/tools/bin/3dsxtool $< $@
clean:
	rm -rf build
