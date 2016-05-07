#time ./ray -w 512 -r 3 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png scenes/polymesh/dragon5.ray out.bmp
#time ./ray -w 512 -x 4 -d -r 4 -f 0.95 -p 10000000 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png sphere_refract4.ray out_sphere_test.bmp

time ./ray -w 512 -x 4 -d -r 4 -f 100 -p 10000000 simple-sphere-caustics-test.ray out2.bmp

#time ./ray -w 512 -r 3 -f 100 -p 1 -L cubemaps/cubemap_autumn/autumn_negative_x.png -T cubemaps/cubemap_autumn/autumn_positive_y.png -F cubemaps/cubemap_autumn/autumn_positive_z.png -D cubemaps/cubemap_autumn/autumn_negative_y.png -R cubemaps/cubemap_autumn/autumn_positive_x.png -B cubemaps/cubemap_autumn/autumn_negative_z.png sphere_refract4.ray out_sphere_refract.bmp
#time ./ray -w 512 -r 3 -f 100 -p 100000 box.ray out_sphere_refract.bmp
#time ./ray -w 512 -r 3 -f 0.5 -p 1000000 box.ray out_sphere_refract.bmp
#time ./ray -w 512 -r 3 -f 0.5 -p 1000000 sphere_refract4.ray out_sphere_refract.bmp
