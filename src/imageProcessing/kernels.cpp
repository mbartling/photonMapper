#include "kernels.h"
#define PI 3.14159265
#include <math.h>

double Gauss2d(int x, int y){
  return exp(-0.5*(x*x + y*y))/(2*PI);
}

void createGaussKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer){
  double mSum = 0.0;
  int x2 = m_conv_x/2;
  int y2 = m_conv_y/2;

  for(int i = 0; i < m_conv_x; i++){
    for(int j = 0; j < m_conv_y; j++){
      double myDouble = Gauss2d(i - x2, j - y2)*255;
      buffer[i*m_conv_x + j] = myDouble;
      mSum += myDouble;
    }
  }
  
  m_div = mSum;
}
void createMeanKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer){
  double mSum = 0.0;
  for(int i = 0; i < m_conv_x; i++){
    for(int j = 0; j < m_conv_y; j++){
      buffer[i*m_conv_x + j] = 1;
      mSum += 1;
    }
  }
  m_div = mSum;

}
// I know the 3x3 but general nxn are non standard.
//http://rsbweb.nih.gov/nih-image/download/user-macros/slowsobel.macro
void createSobXKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer){
  m_div = 1;
  if(m_conv_x == 3 && m_conv_y == 3){
    double temp[] = {-1, 0, 1,
                     -2, 0, 2,
                     -1, 0, 1 };
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}
  }else if(m_conv_x == 5 && m_conv_y == 5){
    double temp[] = {-2, -1, 0, 1, 2,
                     -3, -2, 0, 2, 3,
                     -4, -3, 0, 3, 4,
                     -3, -2, 0, 2, 3, 
                     -2, -1, 0, 1, 2};
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}

  }else if(m_conv_x == 7 && m_conv_y == 7){
    double temp[] = {-3, -2, -1, 0, 1, 2, 3,
                     -4, -3, -2, 0, 2, 3, 4,
                     -5, -4, -3, 0, 3, 4, 5,
                     -6, -5, -4, 0, 4, 5, 6,
                     -5, -4, -3, 0, 3, 4, 5,
                     -4, -3, -2, 0, 2, 3, 4,
                     -3, -2, -1, 0, 1, 2, 3,
                     };
    
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}
  } else{
    for(int i = 0; i < m_conv_x; i++){
      for(int j = 0; j < m_conv_y; j++){
        if(i == m_conv_x/2 && j == m_conv_y/2){
          buffer[i*m_conv_x + m_conv_y] = 1;
        } else{
          buffer[i*m_conv_x + m_conv_y] = 1;
        }
      }
    }
  }// End Else
}
void createSobYKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer){
  m_div = 1;
  if(m_conv_x == 3 && m_conv_y == 3){
    double temp[] = { 1 , 2,  1,
                      0 , 0,  0,
                     -1 ,-2, -1 };
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}
  }else if(m_conv_x == 5 && m_conv_y == 5){
    double temp[] = { 2,  3,  4,  3,  2,
                      1,  2,  3,  2,  1,
                      0,  0,  0,  0,  0,
                     -1, -2, -3, -2, -1,
                     -2, -3, -4, -3, -2};
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}

  }else if(m_conv_x == 7 && m_conv_y == 7){
    double temp[] = { 3,  4,  5,  6,  5,  4,  3,
                      2,  3,  4,  5,  4,  3,  2,
                      1,  2,  3,  4,  3,  2,  1,
                      0,  0,  0,  0,  0,  0,  0,
                     -1, -2, -3, -4, -3, -2, -1,
                     -2, -3, -4, -5, -4, -3, -2,
                     -3, -4, -5, -6, -5, -4, -3
                     };
    
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}
  } else{
    for(int i = 0; i < m_conv_x; i++){
      for(int j = 0; j < m_conv_y; j++){
        if(i == m_conv_x/2 && j == m_conv_y/2){
          buffer[i*m_conv_x + m_conv_y] = 1;
        } else{
          buffer[i*m_conv_x + m_conv_y] = 1;
        }
      }
    }
  }// End Else

}

void createLaplacianKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer){
  m_div = 1;
  if(m_conv_x == 3 && m_conv_y == 3){

    double temp[] = { 0 , 1,  0,
                      1 , -4,  1,
                     0 ,1, 0 };
    for(int i = 0; i < 9; i++){buffer[i] = temp[i];}

  } else{
    for(int i = 0; i < m_conv_x; i++){
      for(int j = 0; j < m_conv_y; j++){
        if(i == m_conv_x/2 && j == m_conv_y/2){
          buffer[i*m_conv_x + m_conv_y] = 1;
        } else{
          buffer[i*m_conv_x + m_conv_y] = 1;
        }
      }
    }
  }// End Else

}