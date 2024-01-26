#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define K 64
#define FILE_NAME_LEN_MAX 20
#define LINE_MAX 100
#define OP_TOTAL_MAX 3
#define OP_LEN_MAX 10

typedef struct
{
    bool is_valid;
    uint64_t tag;
    char *block;
    int timestamp;
} CacheLine;

void new_cache_line(int s, int b, CacheLine *cl)
{
    cl->is_valid = false;
    cl->tag = 0;
    cl->block = (void *)malloc(1 << b);
    cl->timestamp = 0;
}

typedef struct
{
    int s;
    int E;
    int b;
    int t;
    char file_name[FILE_NAME_LEN_MAX];
    bool v;
} CommandLineArg;

void parse_command_line_arg(int argc, char *argv[], CommandLineArg *cla)
{
    cla->v = false;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 && ++i < argc)
            cla->s = atoi(argv[i]);
        else if (strcmp(argv[i], "-E") == 0 && ++i < argc)
            cla->E = atoi(argv[i]);
        else if (strcmp(argv[i], "-b") == 0 && ++i < argc)
            cla->b = atoi(argv[i]);
        else if (strcmp(argv[i], "-t") == 0 && ++i < argc)
        {
            for (int n = 0; n < FILE_NAME_LEN_MAX && argv[i][n] != '\0'; n++)
                cla->file_name[n] = argv[i][n];
        }
        else if (strcmp(argv[i], "-v") == 0)
            cla->v = true;
    }
    cla->t = K - cla->s - cla->b;
}

typedef struct
{
    char line[LINE_MAX];
    char op;
    int size;
    uint64_t addr;
    uint64_t tag;
    uint64_t set;
} TraceLine;

void parse_trace_line(char line[100], int s, int b, TraceLine *tl)
{
    char addr_[16] = {0}; // Address is 64-bit, need 16 Bytes (one char represents one hex digit).
    int end = 3;
    for (; line[end] != ','; end++)
        ;
    strcpy(tl->line, line);
    tl->op = line[1];
    tl->size = atoi(&line[end + 1]);
    strncat(addr_, line + 3, end - 3);
    sscanf(addr_, "%lx", &tl->addr);
    tl->tag = tl->addr >> (s + b);
    tl->set = (tl->addr >> b) & ((1 << s) - 1);
}

int search(CacheLine *cache, int E, uint64_t tag)
{
    for (int i = 0; i < E; i++)
    {
        if (cache[i].is_valid && cache[i].tag == tag)
            return i;
    }
    return -1;
}

int load_(CacheLine *cache, int E, uint64_t tag)
{
    for (int i = 0; i < E; i++)
    {
        if (!cache[i].is_valid)
        {
            cache[i].is_valid = true;
            cache[i].tag = tag;
            return i;
        }
    }
    return -1;
}

int evict(CacheLine *cache, int E, uint64_t tag)
{
    int index = 0, min_clock = cache[0].timestamp;
    for (int i = 1; i < E; i++)
    {
        if (cache[i].timestamp < min_clock)
        {
            index = i;
            min_clock = cache[i].timestamp;
        }
    }
    cache[index].tag = tag;
    return index;
}

void print_cache(CacheLine **cache, int s, int E)
{
    for (int i = 0; i < (1 << s); i++)
    {
        for (int j = 0; j < E; j++)
        {
            int index = i * E + j;
            printf("set: %d, is_valid: %d, tag: %lx, timestamp: %d\n", i, cache[index]->is_valid, cache[index]->tag, cache[index]->timestamp);
        }
    }
}

void print_detail(TraceLine *tl, char result[OP_TOTAL_MAX][OP_LEN_MAX])
{
    int end = 0;
    char line_[LINE_MAX] = {0};
    strcat(line_, tl->line + 1); // Remove the preceding space.
    while (line_[end] != 0 && end < LINE_MAX)
        end++;
    if (end == LINE_MAX) // Error.
        return;
    for (line_[end - 1] = 32; end < LINE_MAX; end++)
    {
        for (int i = 0, j = 0; i < OP_TOTAL_MAX; i++, j = 0)
        {
            while (result[i][j] != 0 && j < OP_LEN_MAX)
                line_[end++] = result[i][j++];
            if (end < LINE_MAX)
                line_[end++] = 32;
        }
    }
    printf("%s\n", line_);
}

void load_or_store(CacheLine *cl, CommandLineArg *cla, TraceLine *tl, int clock, int *hits, int *misses, int *evictions,
                   char result[OP_TOTAL_MAX][OP_LEN_MAX], int *res_i)
{
    int index = search(cl, cla->E, tl->tag);
    strcpy(result[(*res_i)++], index >= 0 ? "hit" : "miss");
    index >= 0 ? (*hits)++ : (*misses)++;

    if (index < 0)
    { // Miss
        index = load_(cl, cla->E, tl->tag);
        if (index < 0)
        {
            index = evict(cl, cla->E, tl->tag);
            strcpy(result[(*res_i)++], "eviction");
            (*evictions)++;
        }
    }

    cl[index].timestamp = clock;
}

void modify(CacheLine *cl, CommandLineArg *cla, TraceLine *tl, int clock, int *hits, int *misses, int *evictions,
            char result[OP_TOTAL_MAX][OP_LEN_MAX], int *res_i)
{
    load_or_store(cl, cla, tl, clock, hits, misses, evictions, result, res_i); // Load
    load_or_store(cl, cla, tl, clock, hits, misses, evictions, result, res_i); // Store
}

int main(int argc, char *argv[])
{
    /* Parse command-line arguments. */
    CommandLineArg *cla = (CommandLineArg *)malloc(sizeof(CommandLineArg));
    parse_command_line_arg(argc, argv, cla);

    printf("s: %d, E: %d, b: %d, v: %d, t: %d, file_name: %s\n", cla->s, cla->E, cla->b, cla->v, cla->t, cla->file_name);

    /* Init cache. */
    int cache_line_total = (1 << cla->s) * cla->E;
    CacheLine **cache = (CacheLine **)malloc(cache_line_total * sizeof(CacheLine *));
    for (int i = 0; i < cache_line_total; i++)
    {
        cache[i] = (CacheLine *)malloc(sizeof(CacheLine));
        new_cache_line(cla->s, cla->b, cache[i]);
    }

    /* Parse trace file. */
    FILE *file = fopen(cla->file_name, "r");
    char line[LINE_MAX];
    int hits = 0, misses = 0, evictions = 0;
    for (int clock = 0; fgets(line, sizeof(line), file); clock++)
    {
        if (line[0] != 32)
            continue;

        TraceLine *tl = (TraceLine *)malloc(sizeof(TraceLine));
        parse_trace_line(line, cla->s, cla->b, tl);
        int base = tl->set * cla->E;
        CacheLine *cl = *(cache + base);
        char result[OP_TOTAL_MAX][OP_LEN_MAX] = {"", "", ""};
        int res_i = 0;

        if (tl->op == 'L')
            load_or_store(cl, cla, tl, clock, &hits, &misses, &evictions, result, &res_i);
        else if (tl->op == 'M')
            modify(cl, cla, tl, clock, &hits, &misses, &evictions, result, &res_i);
        else if (tl->op == 'S')
            load_or_store(cl, cla, tl, clock, &hits, &misses, &evictions, result, &res_i);

        if (cla->v)
            print_detail(tl, result);

        free(tl);
    }
    fclose(file);
    printSummary(hits, misses, evictions);

    return 0;
}
