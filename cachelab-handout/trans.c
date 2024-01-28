/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void solve_32_32(int M, int N, int A[N][M], int B[M][N])
{
    for (int i = 0; i < N; i += 8)
    {
        for (int j = 0; j < M; j += 8)
        {
            for (int k = 0; k < 8; k++)
            {
                int v0 = A[i + k][j];
                int v1 = A[i + k][j + 1];
                int v2 = A[i + k][j + 2];
                int v3 = A[i + k][j + 3];
                int v4 = A[i + k][j + 4];
                int v5 = A[i + k][j + 5];
                int v6 = A[i + k][j + 6];
                int v7 = A[i + k][j + 7];

                B[j][i + k] = v0;
                B[j + 1][i + k] = v1;
                B[j + 2][i + k] = v2;
                B[j + 3][i + k] = v3;
                B[j + 4][i + k] = v4;
                B[j + 5][i + k] = v5;
                B[j + 6][i + k] = v6;
                B[j + 7][i + k] = v7;
            }
        }
    }
}

void solve_64_64(int M, int N, int A[N][M], int B[M][N])
{
    for (int i = 0; i < N; i += 8)
    {
        for (int j = 0; j < M; j += 8)
        {
            int v0, v1, v2, v3, v4, v5, v6, v7;
            for (int k = 0; k < 4; k += 1)
            {
                v0 = A[i + k][j];
                v1 = A[i + k][j + 1];
                v2 = A[i + k][j + 2];
                v3 = A[i + k][j + 3];
                v4 = A[i + k][j + 4];
                v5 = A[i + k][j + 5];
                v6 = A[i + k][j + 6];
                v7 = A[i + k][j + 7];

                B[j][i + k] = v0;
                B[j + 1][i + k] = v1;
                B[j + 2][i + k] = v2;
                B[j + 3][i + k] = v3;
                B[j][i + 4 + k] = v4;
                B[j + 1][i + 4 + k] = v5;
                B[j + 2][i + 4 + k] = v6;
                B[j + 3][i + 4 + k] = v7;
            }

            for (int k = 0; k < 4; k += 1)
            {
                v0 = B[j + k][i + 4];
                v1 = B[j + k][i + 5];
                v2 = B[j + k][i + 6];
                v3 = B[j + k][i + 7];

                v4 = A[i + 4][j + k];
                v5 = A[i + 5][j + k];
                v6 = A[i + 6][j + k];
                v7 = A[i + 7][j + k];

                B[j + k][i + 4] = v4;
                B[j + k][i + 5] = v5;
                B[j + k][i + 6] = v6;
                B[j + k][i + 7] = v7;

                B[j + 4 + k][i] = v0;
                B[j + 4 + k][i + 1] = v1;
                B[j + 4 + k][i + 2] = v2;
                B[j + 4 + k][i + 3] = v3;
            }

            for (int k = 0; k < 4; k += 1)
            {
                v0 = A[i + 4 + k][j + 4];
                v1 = A[i + 4 + k][j + 5];
                v2 = A[i + 4 + k][j + 6];
                v3 = A[i + 4 + k][j + 7];

                B[j + 4][i + 4 + k] = v0;
                B[j + 5][i + 4 + k] = v1;
                B[j + 6][i + 4 + k] = v2;
                B[j + 7][i + 4 + k] = v3;
            }
        }
    }
}

/* M = 61 N = 67 */
void solve(int M, int N, int A[N][M], int B[M][N])
{
    int LEN = 17;

    for (int i = 0; i < (M / LEN + 1) * LEN; i += LEN)
    {
        for (int j = 0; j < (N / LEN + 1) * LEN; j += LEN)
        {
            for (int k = 0; k < LEN && i + k < M; k++)
            {
                for (int l = 0; l < LEN && j + l < N; l++)
                    B[i + k][j + l] = A[j + l][i + k];
            }
        }
    }
}

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32)
        solve_32_32(M, N, A, B);
    else if (M == 64 && N == 64)
        solve_64_64(M, N, A, B);
    else
        solve(M, N, A, B);
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
