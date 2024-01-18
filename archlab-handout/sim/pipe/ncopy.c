#include <stdio.h>

typedef word_t word_t;

word_t src[8], dst[8];

/* $begin ncopy */
/*
 * ncopy - copy src to dst, returning number of positive ints
 * contained in src array.
 */
word_t ncopy(word_t *src, word_t *dst, word_t len)
{
    word_t count = 0;
    word_t val;

    while (len > 0)
    {
        val = *src++;
        *dst++ = val;
        if (val > 0)
            count++;
        len--;
    }
    return count;
}
/* $end ncopy */

word_t ncopy_2(word_t *src, word_t *dst, word_t len)
{
    word_t count = 0;
    word_t count_1 = 0;
    word_t count_2 = 0;
    word_t val;

    while (len - 2 >= 0)
    {
        val = *src++;
        *dst++ = val;
        if (val > 0)
            count_1++;

        val = *src++;
        *dst++ = val;
        if (val > 0)
            count_2++;

        len -= 2;
    }

    while (len > 0)
    {
        val = *src++;
        *dst++ = val;
        if (val > 0)
            count++;
        len--;
    }

    count = count + count_1 + count_2;

    return count;
}

int main()
{
    word_t i, count;

    for (i = 0; i < 8; i++)
        src[i] = i + 1;
    count = ncopy(src, dst, 8);
    printf("count=%d\n", count);
    exit(0);
}
