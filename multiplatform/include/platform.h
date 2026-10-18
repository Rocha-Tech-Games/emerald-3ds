#ifndef WORLD_PLATFORM_H
#define WORLD_PLATFORM_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Proposed backend boundary. No implementation or engine connection yet.
   Each frame supplies both logical surfaces; PC/Switch compose them, 3DS
   presents them separately. Buffers are borrowed only during the call. */
typedef struct { const uint16_t *pixels; unsigned width,height,stride; } WorldSurface;
typedef struct {
    uint32_t (*buttons)(void *context);
    void (*present)(void *context, const WorldSurface *top, const WorldSurface *bottom);
    void (*audio)(void *context, const int16_t *stereo, size_t frames);
    uint64_t (*monotonic_us)(void *context);
    /* Must preserve previous generation on failure; caller uses logical slots,
       never platform paths. Persistent adapter implementation still pending. */
    bool (*commit_save)(void *context, const uint8_t *bytes, size_t length);
    void *context;
} WorldPlatform;
#endif
