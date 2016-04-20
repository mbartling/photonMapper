#!/bin/bash
echo "Enter CubeMapdir rayfile"
time ./ray -w 512 -r 3 -x 2 -L $1/negx.png -T $1/posy.png -F $1/posz.png -D $1/negy.png -R $1/posx.png -B $1/negz.png $2 out.bmp

#time ./ray -w 512 -r 3 -x 4 -d -L $1/negx.png -T $1/posy.png -F $1/posz.png -D $1/negy.png -R $1/posx.png -B $1/negz.png $2 out.bmp
