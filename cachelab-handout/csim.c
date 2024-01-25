#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define K 64
#define FILE_NAME_LEN_MAX 20

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

void parse_command_line_arg(int argc, char *argv[], int *s, int *E, int *b, int *t, char *file_name, bool *v)
{
    *v = false;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 && ++i < argc)
            *s = atoi(argv[i]);
        else if (strcmp(argv[i], "-E") == 0 && ++i < argc)
            *E = atoi(argv[i]);
        else if (strcmp(argv[i], "-b") == 0 && ++i < argc)
            *b = atoi(argv[i]);
        else if (strcmp(argv[i], "-t") == 0 && ++i < argc)
        {
            for (int n = 0; n < FILE_NAME_LEN_MAX && argv[i][n] != '\0'; n++)
                file_name[n] = argv[i][n];
        }
        else if (strcmp(argv[i], "-v") == 0)
            *v = true;
    }
    *t = K - *s - *b;
}

void parse_trace_line(char line[100], int s, int b, char *op, uint64_t *addr, int *size, uint64_t *tag, uint64_t *set)
{
    char addr_[16] = {0}; // Address is 64-bit, need 16 Bytes (one char represents one hex digit).
    int end = 3;
    for (; line[end] != ','; end++)
        ;
    *op = line[1];
    *size = atoi(&line[end + 1]);
    strncat(addr_, line + 3, end - 3);
    sscanf(addr_, "%lx", addr);
    *tag = *addr >> (s + b);
    *set = (*addr >> b) & ((1 << s) - 1);
}

int main(int argc, char *argv[])
{
    int s, E, b, t;
    char file_name[FILE_NAME_LEN_MAX];
    bool v;
    parse_command_line_arg(argc, argv, &s, &E, &b, &t, file_name, &v);

    printf("s: %d, E: %d, b: %d, v: %d, t: %d, file_name: %s\n", s, E, b, v, t, file_name);

    // Init cache.
    int cache_line_total = (1 << s) * E;
    struct cache_line *cache = (struct cache_line *)malloc(cache_line_total * sizeof(struct cache_line));
    for (int i = 0; i < cache_line_total; i++)
        cache[i] = new_cache_line(s, b);

    // Parse trace file.
    FILE *file = fopen(file_name, "r");
    char line[100];
    for (int clock = 0; fgets(line, sizeof(line), file); clock++)
    {
        if (line[0] != 32)
            continue;

        printf("line: %s", line);

        int size;
        uint64_t addr, set, tag;
        char op;
        parse_trace_line(line, s, b, &op, &addr, &size, &tag, &set);

        int base = set * E;
        bool hit = false, miss = false, eviction = false;
        if (op == 'L')
        {
            for (int i = 0; i < E; i++)
            {
                if (cache[base + i].is_valid && cache[base + i].tag == tag)
                {
                    hit = true;
                    cache[base + i].timestamp = clock;
                    break;
                }
            }
            if (!hit)
            {
                miss = true;
                int i = 0;
                for (; i < E; i++)
                {
                    if (!cache[base + i].is_valid)
                    {
                        cache[base + i].is_valid = true;
                        cache[base + i].timestamp = clock;
                        break;
                    }
                }
                if (i == E)
                {
                    eviction = true;
                    int index = 0, min_clock = cache[base].timestamp;
                    for (int i = 1; i < E; i++)
                    {
                        if (cache[base + i].timestamp < min_clock)
                        {
                            index = i;
                            min_clock = cache[base + i].timestamp;
                        }
                    }
                    cache[base + index].timestamp = clock;
                }
            }
        }
        else if (op == 'M')
        {
            for (int i = 0; i < E; i++)
            {
                if (cache[base + i].is_valid && cache[base + i].tag == tag)
                {
                    hit = true;
                    cache[base + i].timestamp = clock;
                    break;
                }
            }
            if (!hit)
            {
                miss = true;
                int i = 0;
                for (; i < E; i++)
                {
                    if (!cache[base + i].is_valid)
                    {
                        cache[base + i].is_valid = true;
                        cache[base + i].timestamp = clock;
                        break;
                    }
                }
                if (i == E)
                {
                    eviction = true;
                    int index = 0, min_clock = cache[base].timestamp;
                    for (int i = 1; i < E; i++)
                    {
                        if (cache[base + i].timestamp < min_clock)
                        {
                            index = i;
                            min_clock = cache[base + i].timestamp;
                        }
                    }
                    cache[base + index].timestamp = clock;
                }
            }
        }
        else if (op == 'S')
        {
            for (int i = 0; i < E; i++)
            {
                if (cache[base + i].is_valid && cache[base + i].tag == tag)
                {
                    hit = true;
                    cache[base + i].timestamp = clock;
                    break;
                }
            }
        }

        printf("op: %c, size: %d, addr: %lX, set: %lX, tag: %lX\n", op, size, addr, set, tag);
        printf("hit: %d, miss: %d, eviction: %d\n", hit, miss, eviction);
    }
    fclose(file);

    for (int i = 0; i < cache_line_total; i++)
        free(cache[i].block);
    free(cache);

    printSummary(0, 0, 0);

    return 0;
}
