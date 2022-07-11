//南希 2000012824


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
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    //对32*32,采用8*8分块，并且先用8个变量保存从A这个数组读出的内容
    if(M==32&&N==32)
    {
        int val1, val2, val3, val4, val5, val6, val7, val8;
        for (int m = 0; m < 4; m++)
            for (int s = 0; s < 4; s++)
            {
                for (int n = 0; n < 8; n++)
                {
                    val1 = A[8 * m + n][8 * s];
                    val2 = A[8 * m + n][8 * s + 1];
                    val3 = A[8 * m + n][8 * s + 2];
                    val4 = A[8 * m + n][8 * s + 3];
                    val5 = A[8 * m + n][8 * s + 4];
                    val6 = A[8 * m + n][8 * s + 5];
                    val7 = A[8 * m + n][8 * s + 6];
                    val8 = A[8 * m + n][8 * s + 7];
                    B[8 * s][8 * m + n] = val1;
                    B[8 * s + 1][8 * m + n] = val2;
                    B[8 * s + 2][8 * m + n] = val3;
                    B[8 * s + 3][8 * m + n] = val4;
                    B[8 * s + 4][8 * m + n] = val5;
                    B[8 * s + 5][8 * m + n] = val6;
                    B[8 * s + 6][8 * m + n] = val7;
                    B[8 * s + 7][8 * m + n] = val8;
                }
            }
    }
    /*对64*64，采用8*8分块
    每个块分成4个部分，先主要处理左上角的4*4分块，
    对每个块的前四行，先读一整行8个数，将前4个放在B中对应的地方上，
    将后4个先将这一行读进来的转成一列，再将这一列放在B中的右上角
    对B的右上角和左下角，先从A中读出左下角的4*4的块的一列，
    再将B的右上角的一行读出来，将A中左下角读出的一列的数放到B中对应的地方
    将B右上角的一行放在B的左下角对应的地方
    最后对A的右下角进行操作，将对应的元素放到B的右下角去
    */
    if (M ==64 && N == 64)
    {
        int val1, val2, val3, val4, val5, val6, val7, val8, i,s,n,m;
        for (m = 0; m < 8; m++)
            for (s = 0; s < 8; s++)
            {
                for (n = 0; n < 4; n++)
                {
                    i = 8 * m+n;
                    val1 = A[i][8 * s];
                    val2 = A[i][8 * s + 1];
                    val3 = A[i][8 * s + 2];
                    val4 = A[i][8 * s + 3];
                    val5 = A[i][8 * s + 4];
                    val6 = A[i][8 * s + 5];
                    val7 = A[i][8 * s + 6];
                    val8 = A[i][8 * s + 7];
                    B[8 * s][i] = val1;
                    B[8 * s + 1][i] = val2;
                    B[8 * s + 2][i] = val3;
                    B[8 * s + 3][i] = val4;
                    B[8 * s][i + 4] = val5;
                    B[8 * s + 1][i + 4] = val6;
                    B[8 * s + 2][i + 4] = val7;
                    B[8 * s + 3][i + 4] = val8;
                }
                for (n = 0; n < 4; n++)
                {
                    i = 8 * m +4;
                    val1 = A[i][8 * s + n];
                    val2 = A[i + 1][8 * s + n];
                    val3 = A[i + 2][8 * s + n];
                    val4 = A[i + 3][8 * s + n];
                    val5 = B[8 * s + n][i ];
                    val6 = B[8 * s + n][i + 1];
                    val7 = B[8 * s + n][i + 2];
                    val8 = B[8 * s + n][i + 3];
                    B[8 * s + n][i] = val1;
                    B[8 * s + n][i + 1] = val2;
                    B[8 * s + n][i + 2] = val3;
                    B[8 * s + n][i + 3] = val4;
                    B[8 * s + n + 4][i - 4] = val5;
                    B[8 * s + n + 4][i - 3] = val6;
                    B[8 * s + n + 4][i - 2] = val7;
                    B[8 * s + n + 4][i - 1] = val8;
                }
                for (n = 0; n < 4; n += 2)
                {
                    i = 8 * m +4+n;
                    val1 = A[i][8 * s + 4];
                    val2 = A[i][8 * s + 4 + 1];
                    val3 = A[i][8 * s + 4 + 2];
                    val4 = A[i][8 * s + 4 + 3];
                    val5 = A[i + 1][8 * s + 4];
                    val6 = A[i + 1][8 * s + 4 + 1];
                    val7 = A[i + 1][8 * s + 4 + 2];
                    val8 = A[i + 1][8 * s + 4 + 3];
                    B[8 * s + 4][i] = val1;
                    B[8 * s + 4 + 1][i] = val2;
                    B[8 * s + 4 + 2][i] = val3;
                    B[8 * s + 4 + 3][i] = val4;
                    B[8 * s + 4][i + 1] = val5;
                    B[8 * s + 4 + 1][i + 1] = val6;
                    B[8 * s + 4 + 2][i + 1] = val7;
                    B[8 * s + 4 + 3][i + 1] = val8;
                }
                
            }
    }
    //采用8*4展开，之后再4*4处理剩下的内容
    if (M == 60 && N == 68)
    {
        int ii,jj,  i, j, val1, val2, val3, val4;
        for (ii = 0; ii + 8 < N; ii += 8)
            for (jj = 0; jj + 4 <= M; jj += 4)
            {
                for (i = ii; i < ii + 8; i++)
                {
                    val1 = A[i][jj + 0];
                    val2 = A[i][jj + 1];
                    val3 = A[i][jj + 2];
                    val4 = A[i][jj + 3];
                    B[jj + 0][i] = val1;
                    B[jj + 1][i] = val2;
                    B[jj + 2][i] = val3;
                    B[jj + 3][i] = val4;
                }
            }        
        
        for (j = 0; j < M; j += 4)
            for (i = ii; i < ii + 4; i++)
            {
                val1 = A[i][j + 0];
                val2 = A[i][j + 1];
                val3 = A[i][j + 2];
                val4 = A[i][j + 3];
                B[j + 0][i] = val1;
                B[j + 1][i] = val2;
                B[j + 2][i] = val3;
                B[j + 3][i] = val4;
            }
    }
    ENSURES(is_transpose(M, N, A, B));
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

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
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

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
