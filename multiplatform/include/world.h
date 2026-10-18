#ifndef WORLD_H
#define WORLD_H
#include <stddef.h>
#include <stdint.h>
#define WORLD_FLASH_SIZE 131072u
#define WORLD_HEADER_SIZE 32u
#define WORLD_SAVE_SIZE (WORLD_HEADER_SIZE + WORLD_FLASH_SIZE)
/* Stable wire IDs; never reorder or reuse. All region content is pending. */
enum WorldRegion { WORLD_KANTO=1, WORLD_JOHTO, WORLD_HOENN, WORLD_SINNOH, WORLD_UNOVA };
const char *world_region_name(unsigned id);
/* No allocation, disk access, native structs or platform pointers on the wire.
   Source and destination must not overlap. Returns 0 on success. */
int world_wrap(uint8_t *out, size_t capacity, const uint8_t *flash, size_t length);
int world_validate(const uint8_t *save, size_t length);
#endif
