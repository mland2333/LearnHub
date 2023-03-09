/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "ovik",              /* Team name */

    "Ml Q. Bovik",     /* First member full name */
    "mlvik@nowhere.edu",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    //int a0,a1,a2,a3,a4,a5,a6,a7;
    //int k = dim>>3;
    for (i = 0; i < dim; i+=8)
	    for (j = 0; j < dim; j+=8)
        {
            for(int x = i;x<8+i;++x)
            {
                //int y=j;
                dst[RIDX(dim-1-j, x, dim)] = src[RIDX(x, j, dim)];
                dst[RIDX(dim-2-j, x, dim)] = src[RIDX(x, j+1, dim)]; 
                dst[RIDX(dim-3-j, x, dim)] = src[RIDX(x, j+2, dim)]; 
                dst[RIDX(dim-4-j, x, dim)] = src[RIDX(x, j+3, dim)]; 
                dst[RIDX(dim-5-j, x, dim)] = src[RIDX(x, j+4, dim)]; 
                dst[RIDX(dim-6-j, x, dim)] = src[RIDX(x, j+5, dim)]; 
                dst[RIDX(dim-7-j, x, dim)] = src[RIDX(x, j+6, dim)]; 
                dst[RIDX(dim-8-j, x, dim)] = src[RIDX(x, j+7, dim)];
                /*dst[RIDX(dim-9-j, x, dim)] = src[RIDX(x, j+8, dim)];
                dst[RIDX(dim-10-j, x, dim)] = src[RIDX(x, j+9, dim)]; 
                dst[RIDX(dim-11-j, x, dim)] = src[RIDX(x, j+10, dim)]; 
                dst[RIDX(dim-12-j, x, dim)] = src[RIDX(x, j+11, dim)]; 
                dst[RIDX(dim-13-j, x, dim)] = src[RIDX(x, j+12, dim)]; 
                dst[RIDX(dim-14-j, x, dim)] = src[RIDX(x, j+13, dim)]; 
                dst[RIDX(dim-15-j, x, dim)] = src[RIDX(x, j+14, dim)]; 
                dst[RIDX(dim-16-j, x, dim)] = src[RIDX(x, j+15, dim)];*/
            } 
            //dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];

        }
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    naive_rotate(dim, src, dst);
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *******************************   **************************************/

void register_rotate_functions() 
{                               
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    /* ... Register ad ditional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs a nd helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;


static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum -  Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;
     sum.red = sum.green = sum.blue = 0;
    sum.num = 0;
    //initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	{
        sum.red += (int) src[RIDX(ii,jj,dim)].red;
        sum.green += (int) src[RIDX(ii,jj,dim)].green;
        sum.blue += (int) src[RIDX(ii,jj,dim)].blue;  
        sum.num++;
    }
    current_pixel.red = (unsigned short) (sum.red/sum.num);
    current_pixel.green = (unsigned short) (sum.green/sum.num);
    current_pixel.blue = (unsigned short) (sum.blue/sum.num);
    //assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int add=1;
    for (i = 0; i < dim; i+=add)
	for (j = 0; j < dim; j+=add)
    {
        /*for(int x = i;x<i+add;++x)
        {
	        dst[RIDX(x, j, dim)] = avg(dim, x, j, src);
            dst[RIDX(x, j+1, dim)] = avg(dim, x, j+1, src);
            dst[RIDX(x, j+2, dim)] = avg(dim, x, j+2, src);
            dst[RIDX(x, j+3, dim)] = avg(dim, x, j+3, src);
            dst[RIDX(x, j+4, dim)] = avg(dim, x, j+4, src);
            dst[RIDX(x, j+5, dim)] = avg(dim, x, j+5, src);
            dst[RIDX(x, j+6, dim)] = avg(dim, x, j+6, src);
            dst[RIDX(x, j+7, dim)] = avg(dim, x, j+7, src);
        }*/
        dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
    }
}
/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    naive_smooth(dim, src, dst);
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

