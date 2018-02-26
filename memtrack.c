#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct {
    const char *file;
    int line;
} location;


typedef struct memtrack_chunk {
    void *ptr;
    size_t size;

    location loc_alloc;

    struct memtrack_chunk *next;
} memtrack_chunk;


static memtrack_chunk *chunks = NULL;
bool tracking = false;

void *memtrack_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);

    if (tracking) {
        memtrack_chunk *c = malloc(sizeof(memtrack_chunk));
        c->ptr = ptr;
        c->size = size;
        c->loc_alloc.file = file;
        c->loc_alloc.line = line;

        c->next = chunks;
        chunks = c;
    }

    return ptr;
}


void *memtrack_realloc(void *p, size_t new_size, const char *file, int line) {
    void *ptr = realloc(p, new_size);

    if (!tracking)
        return ptr;

    memtrack_chunk *c =  chunks;
    while (c) {
        if (c->ptr == p) {

            c->ptr = ptr;
            c->size = new_size;
            c->loc_alloc.file = file;
            c->loc_alloc.line = line;

            break;
        }
        c = c->next;
    }

    if (!c) {
        if (p) {
            printf("!!! %s:%d: realloc of a freed/invalid pointer of %lu bytes allocated on %s:%d\n",
                   file, line, c->size,
                   c->loc_alloc.file, c->loc_alloc.line);
        }

        memtrack_chunk *c = malloc(sizeof(memtrack_chunk));
        c->ptr = ptr;
        c->size = new_size;
        c->loc_alloc.file = file;
        c->loc_alloc.line = line;

        c->next = chunks;
        chunks = c;
    }

    return ptr;
}


void memtrack_free(void *p, const char *file, int line) {
    free(p);

    if (!tracking)
        return;

    memtrack_chunk *found = NULL;
    if (chunks->ptr == p) {
        found = chunks;
        chunks = chunks->next;
    } else {
        memtrack_chunk *c = chunks;
        while (c->next) {
            if (c->next->ptr == p) {
                found = c->next;
                c->next = c->next->next;
                break;
            }

            c = c->next;
        }
    }

    if (!found) {
        printf("!!! %s:%d: free of untracked pointer\n", file, line);
        return;
    }

    free(found);
}

void memtrack_clear() {
    memtrack_chunk *c = chunks;
    while (c) {
        memtrack_chunk *next = c->next;
        free(c);
        c = next;
    }
    chunks = NULL;
}

void memtrack_dump() {
    memtrack_chunk *c = chunks;
    while (c) {
        printf("Chunk %5zu allocated on %s:%d\n",
               c->size, c->loc_alloc.file, c->loc_alloc.line);
        c = c->next;
    }
}

void memtrack_start() {
    tracking = true;
}

void memtrack_stop() {
    memtrack_clear();
    tracking = false;
}


char *memtrack_strdup(const char *s, const char *file, int line) {
    char *ptr = strdup(s);

    if (tracking) {
        memtrack_chunk *c = malloc(sizeof(memtrack_chunk));
        c->ptr = ptr;
        c->size = strlen(ptr) + 1;
        c->loc_alloc.file = file;
        c->loc_alloc.line = line;

        c->next = chunks;
        chunks = c;
    }

    return ptr;
}

char *memtrack_strndup(const char *s, size_t n, const char *file, int line) {
    char *ptr = strndup(s, n);

    if (tracking) {
        memtrack_chunk *c = malloc(sizeof(memtrack_chunk));
        c->ptr = ptr;
        c->size = strlen(ptr) + 1;
        c->loc_alloc.file = file;
        c->loc_alloc.line = line;

        c->next = chunks;
        chunks = c;
    }

    return ptr;
}
