#ifndef __MIMAGE_H__
#define __MIMAGE_H__

#include <cmath>

class mImageC {
  private:
    const unsigned char* _sourceBuffer;
    int srcBufferWidth;
    int srcBufferHeight;
    int cPadding = 0;
    int rPadding = 0;

  public:
    mImageC(const unsigned char* sourceBuffer, int srcBufferWidth, int srcBufferHeight): _sourceBuffer(sourceBuffer), srcBufferWidth(srcBufferWidth), srcBufferHeight(srcBufferHeight)
    {}

    unsigned char get(int row, int column, int rgb){

        if(row < rPadding || row >= rPadding + srcBufferHeight || column < cPadding || column >= cPadding + srcBufferWidth){
          return 0;
        } else{
          return _sourceBuffer[3*((row-rPadding)*srcBufferWidth+(column-cPadding))+rgb];
        }

    }

    void zeroPad(int rPad, int cPad){
      cPadding = cPad;
      rPadding = rPad;
    }

    void clearPadding(void){
      cPadding = 0;
      rPadding = 0;
    }

};

class mImage {
  private:
    unsigned char* _sourceBuffer;
    int srcBufferWidth;
    int srcBufferHeight;
    int cPadding = 0;
    int rPadding = 0;

  public:
    mImage(unsigned char* sourceBuffer, int srcBufferWidth, int srcBufferHeight): _sourceBuffer(sourceBuffer), srcBufferWidth(srcBufferWidth), srcBufferHeight(srcBufferHeight)
    {}

    unsigned char get(int row, int column, int rgb){

        if(row < rPadding || row >= rPadding + srcBufferHeight || column < cPadding || column >= cPadding + srcBufferWidth){
          return 0;
        } else{
          return _sourceBuffer[3*((row- rPadding)*srcBufferWidth+(column-cPadding))+rgb];
        }

    }
    void set(int row, int column, int rgb, unsigned char val){

        if(row < rPadding || row >= rPadding + srcBufferHeight || column < cPadding || column >= cPadding + srcBufferWidth){
          return;
        } else{
          _sourceBuffer[3*((row-rPadding)*srcBufferWidth+(column-cPadding))+rgb] = val;
        }

    }

    void zeroPad(int rPad, int cPad){
      cPadding = cPad;
      rPadding = rPad;
    }

    void clearPadding(void){
      cPadding = 0;
      rPadding = 0;
    }

};

#endif
