#ifndef __KERNELS_H__
#define __KERNELS_H__
enum
{
  KERNEL_MEAN = 0,
  KERNEL_WMEAN,
  KERNEL_GAUSS,
  KERNEL_SOBEL_X,
  KERNEL_SOBEL_Y,
  KERNEL_SOBEL_B,
  KERNEL_LAPLACIAN,
  NUM_KERNEL_TYPE // Make sure this stays at the end!
};

double Gauss2d(int x, int y);
void createGaussKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer);
void createMeanKernel( int m_conv_x, int m_conv_y,  double &m_div, double* buffer);
void createSobXKernel( int m_conv_x, int m_conv_y,  double &m_div, double* buffer);
void createSobYKernel( int m_conv_x, int m_conv_y,  double &m_div, double* buffer);
void createLaplacianKernel( int m_conv_x, int m_conv_y, double &m_div, double* buffer);

#endif
