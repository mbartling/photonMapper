#include "filters.h"
// #include <algorithm>

void applyFilter( const unsigned char* sourceBuffer,
    int srcBufferWidth, int srcBufferHeight,
    unsigned char* destBuffer,
    const double *filterKernel, 
    int knlWidth, int knlHeight, 
    double divisor, double offset )
{
  //Note these are also the centers of the kernel
  int zeroPadC = knlWidth / 2;  // size of the zero padding
  int zeroPadR = knlHeight / 2; // Note, will make the edges slightly darker/less blurred
                                // for even sized kernels
  if(divisor == 0) divisor = 1;
  if(std::abs(divisor) < 0.00001){
    if(divisor < 0) divisor = -0.00001;
    if(divisor > 0) divisor = 0.00001;
  }
  mImageC myImageIn(sourceBuffer, srcBufferWidth, srcBufferHeight);
  mImage myImageOut(destBuffer, srcBufferWidth, srcBufferHeight);
  // Zero Pad the Image
  myImageIn.zeroPad(zeroPadR, zeroPadC);
  myImageOut.zeroPad(zeroPadR, zeroPadC);

  // For each element in the image convolve with the filter
  // Assume odd sized filters for simplicity
  for(int columnI = 0; columnI < srcBufferWidth; columnI++ ){
    for(int rowI = 0; rowI < srcBufferHeight; rowI++ ){
      double mSumR = 0;
      double mSumG = 0;
      double mSumB = 0;

      for( int i = 0; i < knlWidth; i++){
        for( int j = 0; j < knlHeight; j++){
          mSumR += filterKernel[i*knlWidth + j]*myImageIn.get(rowI + i, columnI + j, 0);
          mSumG += filterKernel[i*knlWidth + j]*myImageIn.get(rowI + i, columnI + j, 1);
          mSumB += filterKernel[i*knlWidth + j]*myImageIn.get(rowI + i, columnI + j, 2);
        }
      }
      mSumR = mSumR/divisor + offset;
      mSumG = mSumG/divisor + offset;
      mSumB = mSumB/divisor + offset;
      if(mSumR > 255) mSumR = 255;
      if(mSumG > 255) mSumG = 255;
      if(mSumB > 255) mSumB = 255;
      if(mSumR < 0) mSumR = 0;
      if(mSumG < 0) mSumG = 0;
      if(mSumB < 0) mSumB = 0;

      // cout << mSumR;
      myImageOut.set(rowI+zeroPadR, columnI+zeroPadC, 0, (unsigned char)mSumR);
      myImageOut.set(rowI+zeroPadR, columnI+zeroPadC, 1, (unsigned char)mSumG);
      myImageOut.set(rowI+zeroPadR, columnI+zeroPadC, 2, (unsigned char)mSumB);
    }
  }
}

// 3x3 Gaussian
void applyGaussian(const unsigned char* sourceBuffer,
    int srcBufferWidth, int srcBufferHeight,
    unsigned char* destBuffer){

  double kernel[25];
  double divisor;
  createGaussKernel(5,5, divisor, kernel);
  applyFilter(sourceBuffer, srcBufferWidth, srcBufferHeight, destBuffer, kernel, 3,3, divisor, 0.0);
}

void mergeImages(unsigned char* dest, const unsigned char* sourceBuffer1, const unsigned char* sourceBuffer2, int width, int height){

  for(int i = 0; i < 3*width*height; i++){
    unsigned int x1 = sourceBuffer1[i];
    // unsigned int x2 = (sourceBuffer1[i] * sourceBuffer2[i])/255;
    unsigned int x2 = sourceBuffer2[i];
    // unsigned int x = (x1 > x2) ? x1 : x1 + x2;
    // unsigned int x = x1 + x2;
    unsigned int x = 255 - ((255 - x1)*(255 - x2))/255;
    if(x > 255) x = 255;
    dest[i] = (unsigned char) x;
  }
}