#ifndef __FILTERS_H__ 
#define __FILTERS_H__ 

#include "mImage.h"
#include "kernels.h"
#include <cmath>

void applyFilter( const unsigned char* sourceBuffer,
    int srcBufferWidth, int srcBufferHeight,
    unsigned char* destBuffer,
    const double *filterKernel, 
    int knlWidth, int knlHeight, 
    double divisor, double offset );

void applyGaussian(const unsigned char* sourceBuffer,
    int srcBufferWidth, int srcBufferHeight,
    unsigned char* destBuffer);

void mergeImages(unsigned char* dest, const unsigned char* sourceBuffer1, const unsigned char* sourceBuffer2, int width, int height);

#endif
