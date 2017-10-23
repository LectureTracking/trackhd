#! /bin/sh

#gcc -o cropvid cropvid.c `pkg-config --cflags --libs libavformat libavfilter libavcodec libavutil` 
gcc -o cropvid cropvid.c -I/usr/local/include -L/usr/local/lib -lavfilter -lxcb -lxcb-shm -lxcb -lx265 -lx264 -lvpx -lm -lvpx -lm -lvpx -lm -lvpx -lm -ldl -llzma -lbz2 -lz -pthread -lswscale -lm -lpostproc -lavformat  -lxcb -lxcb-shm -lxcb -lx265 -lx264 -lvpx -lm -lvpx -lm -lvpx -lm -lvpx -lm -ldl -llzma -lbz2 -lz -pthread -lavcodec -lvdpau -lX11 -lva -lva-x11 -lX11 -lva -lva-drm -lva -lxcb -lxcb-shm -lxcb -lx265 -lx264 -lvpx -lm -lvpx -lm -lvpx -lm -lvpx -lm -ldl -llzma -lbz2 -lz -pthread -lswresample -lm -lavutil -lm -lvdpau -lX11 -lva -lva-x11 -lX11 -lva -lva-drm -lva
