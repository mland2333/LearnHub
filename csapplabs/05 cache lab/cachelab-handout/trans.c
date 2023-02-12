/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, x, y, a1, a2, a3, a4, a5, a6, a7, a8;
    if(M==32)
    {
        for(i = 0;i<M;i+=8)
        {
            for( j = 0;j<M;j+=8)
            {
                for(x = i;x<i+8;++x)
                {
                    y = j;
                    a1=A[x][y];
                    a2=A[x][y+1];
                    a3=A[x][y+2];
                    a4=A[x][y+3];
                    a5=A[x][y+4];
                    a6=A[x][y+5];
                    a7=A[x][y+6];
                    a8=A[x][y+7];

                    B[y][x] = a1;
                    B[y+1][x] = a2;
                    B[y+2][x] = a3;
                    B[y+3][x] = a4;
                    B[y+4][x] = a5;
                    B[y+5][x] = a6;
                    B[y+6][x] = a7;
                    B[y+7][x] = a8;
                      
                }
            }
        }
    }
    else if(M == 64)
    {
        for(i = 0;i<M;i+=8)
        {
            for(j = 0;j<M;j+=8)
            {
                for(x = i;x<i+4;x++)
                {
                    y = j;
                    a1=A[x][y];
                    a2=A[x][y+1];
                    a3=A[x][y+2];
                    a4=A[x][y+3];
                    a5=A[x][y+4];
                    a6=A[x][y+5];
                    a7=A[x][y+6];
                    a8=A[x][y+7];

                    B[y][x] = a1;
                    B[y+1][x] = a2;
                    B[y+2][x] = a3;
                    B[y+3][x] = a4;
                    B[y][x+4] = a5;
                    B[y+1][x+4] = a6;
                    B[y+2][x+4] = a7;
                    B[y+3][x+4] = a8;
                }
                for(y = j;y<j+4;y++)
                {
                    x = i;
                    a1 = B[y][x+4];
                    a2= B[y][x+5];
                    a3=B[y][x+6];
                    a4=B[y][x+7];

                    B[y][x+4]=A[x+4][y];
                    B[y][x+5]=A[x+5][y];
                    B[y][x+6]=A[x+6][y];
                    B[y][x+7]=A[x+7][y];

                    B[y+4][x]=a1;
                    B[y+4][x+1]=a2;
                    B[y+4][x+2]=a3;
                    B[y+4][x+3]=a4;
                    B[y+4][x+4]=A[x+4][y+4];
                    B[y+4][x+5]=A[x+5][y+4];
                    B[y+4][x+6]=A[x+6][y+4];
                    B[y+4][x+7]=A[x+7][y+4];
                }
            }
        }
    }
    else
    {

        /*for(i = 0;i<N-N%8;i+=8)
        {
            for( j = 0;j<M-M%8;j+=8)
            {
                for(x = i;x<i+8;++x)
                {
                    
                    y = j;
                    a1=A[x][y];
                    a2=A[x][y+1];
                    a3=A[x][y+2];
                    a4=A[x][y+3];
                    a5=A[x][y+4];
                    a6=A[x][y+5];
                    a7=A[x][y+6];
                    a8=A[x][y+7];

                    B[y][x] = a1;
                    B[y+1][x] = a2;
                    B[y+2][x] = a3;
                    B[y+3][x] = a4;
                    B[y+4][x] = a5;
                    B[y+5][x] = a6;
                    B[y+6][x] = a7;
                    B[y+7][x] = a8;
                      
                } 
            }
        }
        
        for(x=0;x<i;++x)
            for(y=j;y<M;++y)
                B[y][x]=A[x][y];
        for(x = i;x<N;++x)
            for(y=0;y<j;++y)
                B[y][x]=A[x][y];
        for(;i<N;++i)
        {
            for(;j<M;++j)
            {
                B[j][i]=A[i][j];
            }
        }
    }*/
    a1=23;
    for(i = 0; i < N; i += a1)
        for(j = 0; j < M; j += a1)
			for(x = i; x < N && x < i + a1; ++x)
				for(y = j; y < M && y < j + a1; ++y)
				{
				   B[y][x]=A[x][y]; 
				}
}
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
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
void registerFunctions() {
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
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
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
