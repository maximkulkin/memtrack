#pragma once

// Start tracking memory allocations
void memtrack_start();
// Stop tracking memory allocations and purge all collected data
void memtrack_stop();

// Clear all collected data
void memtrack_clear();
// Dump all allocated but unfreed memory info
void memtrack_dump();

// Override standard memory functions
void *memtrack_malloc(size_t size, const char *file, int line);
void *memtrack_realloc(void *p, size_t new_size, const char *file, int line);
void memtrack_free(void *p, const char *file, int line);

char *memtrack_strdup(const char *s, const char *file, int line);
char *memtrack_strndup(const char *s, size_t n, const char *file, int line);

#define malloc(size) memtrack_malloc(size, __FILE__, __LINE__)
#define realloc(ptr, size) memtrack_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) memtrack_free(ptr, __FILE__, __LINE__)

#define strdup(s) memtrack_strdup(s, __FILE__, __LINE__)
#define stdndup(s, n) memtrack_strndup(s, n, __FILE__, __LINE__)
