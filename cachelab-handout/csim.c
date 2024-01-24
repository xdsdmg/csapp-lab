#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define K 64

struct cache_line
{
    bool is_valid;
    uint64_t tag;
    char *block;
    int timestamp;
};

struct cache_line new_cache_line(int s, int b)
{
    char *block = (void *)malloc(1 << b);
    struct cache_line cl = {false, 0, block, 0};
    return cl;
}

int main(int argc, char *argv[])
{
    int s = 0, E = 0, b = 0, v = 0, t = 0;
    char *file_name;

    // Parse command line args.
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 && ++i < argc)
            s = atoi(argv[i]);
        else if (strcmp(argv[i], "-E") == 0 && ++i < argc)
            E = atoi(argv[i]);
        else if (strcmp(argv[i], "-b") == 0 && ++i < argc)
            b = atoi(argv[i]);
        else if (strcmp(argv[i], "-t") == 0 && ++i < argc)
            file_name = argv[i];
        else if (strcmp(argv[i], "-v") == 0)
            v = 1;
    }
    t = K - s - b;
    printf("s: %d, E: %d, b: %d, v: %d, t: %d, file_name: %s\n", s, E, b, v, t, file_name);

    // Init cache.
    int cache_line_total = (1 << s) * E;
    struct cache_line *cache = (struct cache_line *)malloc(cache_line_total * sizeof(struct cache_line));
    for (int i = 0; i < cache_line_total; i++)
        cache[i] = new_cache_line(s, b);

    // Parse trace file.
    FILE *file = fopen(file_name, "r");
    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] != 32)
            continue;

        char op = line[1], addr_[16] = {0}; // Address is 64-bit, need 16 Bytes (one char represents one hex digit).
        int size;

        int end = 3;
        for (; line[end] != ','; end++)
            ;

        strncat(addr_, line + 3, end - 3);
        size = atoi(&line[end + 1]);

        printf("line: %s", line);

        uint64_t addr;
        sscanf(addr_, "%lx", &addr);
        uint64_t tag = addr >> (s + b);
        uint64_t set = (addr >> b) & ((1 << s) - 1);

        printf("op: %c, addr_: %s, size: %d, addr: %lX, set: %lX, tag: %lX\n", op, addr_, size, addr, set, tag);
    }
    fclose(file);

    for (int i = 0; i < cache_line_total; i++)
        free(cache[i].block);
    free(cache);

    printSummary(0, 0, 0);

    return 0;
}
