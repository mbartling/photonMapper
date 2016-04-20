#include <stdio.h>
#include <stdlib.h>

#include "zlib.h"
#include "png.h"

#ifdef DEBUG
#  define Trace(x)  {fprintf x ; fflush(stderr); fflush(stdout);}
#else
#  define Trace(x)  ;
#endif

typedef unsigned char   uch;
typedef unsigned short  ush;
typedef unsigned long   ulg;


/* prototypes for public functions in readpng.c */

void png_version_info(void);

int png_init(const char* filename, int &pWidth, int &pHeight);

int png_get_bgcolor(uch *bg_red, uch *bg_green, uch *bg_blue);

uch *png_get_image(double display_exponent, int &pChannels,
                       int &pRowbytes);

void png_cleanup(int free_image_data);
