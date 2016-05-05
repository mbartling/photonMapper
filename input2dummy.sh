#time ./ray -w 512 -r 3 -x 2 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png scenes/polymesh/dragon5.ray out.bmp

time ./ray -w 2014 -r 4 -f 0.7 -p 100000 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png wineglasses.ray out.bmp
#time ./ray -w 512 -r 4 -f 0.1 -p 1000000 easy3a.ray out.bmp

#time ./ray -w 512 -r 3 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png sintel.ray out.bmp
# time ./ray -w 1024 -r 4 -x 2 wineglasses.ray out.bmp
# time ./ray -w 512 -r 3 wineglasses.ray out.bmp
#time ./ray -w 512 -r 3 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png sintel.ray out.bmp
#time ./ray -w 1024 -r 4 -f 100.0 -p 5000000 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png mbox.ray out.bmp
