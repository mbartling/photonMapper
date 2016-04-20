#include "pixelblock.h"

std::vector<pixelBlock> getBlockList(int xBound, int yBound, int xWidth, int yWidth){
  std::vector<pixelBlock> v;

  for(int j = 0; j < yBound; j += yWidth){

    for(int i = 0; i < xBound; i += xWidth){
      int xStride = 0; int yStride = 0;

      // Handle the Boundary case for widths
      if ((xBound - i) < xWidth) xStride = xBound - i;
      else xStride = xWidth;
      if ((yBound - j) < yWidth) yStride = yBound - j;
      else yStride = yWidth;

      v.push_back(pixelBlock(i, i + xStride, j, j + yStride));
    }
  }
  return v;
}
