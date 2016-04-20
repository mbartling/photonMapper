#ifndef __PIXELBLOCK_H__
#define __PIXELBLOCK_H__
#include <vector>

struct pixelBlock{
  int xMin;
  int xMax;
  int yMin;
  int yMax;
  pixelBlock(): xMin(0), xMax(0), yMin(0), yMax(0){}
  pixelBlock(int xMin, int xMax, int yMin, int yMax):xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) {} 
};

std::vector<pixelBlock> getBlockList(int xBound, int yBound, int xWidth, int yWidth);

#endif /*#ifndef __PIXELBLOCK_H__*/