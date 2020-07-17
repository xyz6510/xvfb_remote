# xvfb_remote
remotely view xvfb screen
--------------------------

Xvfb remote
-----------
access remotely Xvfb screen

compiling on ubuntu 20.04
---------------------------------------
sudo apt-get install libavformat-dev
sudo apt-get install libx11-dev
sudo apt-get install mesa-common-dev
sudo apt-get install libxcursor-dev
sudo apt-get install libxtst-dev
sudo apt-get install libxdamage-dev
sudo apt-get install libx264-dev
sudo apt-get install cmake
sudo apt-get install git
sudo apt-get install xvfb
sudo apt-get install fluxbox
sudo apt-get install xterm
---------------------------------------
mkdir libyuv
cd libyuv
git clone https://chromium.googlesource.com/libyuv/libyuv
cd libyuv
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ../
make
sudo make install
---------------------------------------
cd encoder
./makeencoder
cd xwingl
./makexwingl
---------------------------------------
edit ~/.fluxbox/menu

[begin] (fluxbox)
[exec] (terminal) {xterm}
[exec] (firefox) {firefox}
[include] (/etc/X11/fluxbox/fluxbox-menu)
[end]

---------------------------------------
only 24 bit Xvfb not less
horizontal Xvfb resolution must be divisable by 64
in this example 1280x720 is maximal resolution xrandr can add only smaller

xterm -e "Xvfb :15 -nocursor -nolisten tcp -fbdir /run/user/1000 -screen scrn 1280x720x24" &
xterm -e "DISPLAY=:15 fluxbox" &

DISPLAY=:15 xset r rate 200 40

xterm -e "DISPLAY=:15 ./encoder -ip 127.0.0.1 -p 45000 -xm /run/user/1000/Xvfb_screen0 -yuv 444 -ct 1 -yt 1 -xt 1" &

xterm -e "./xwingl -ip 127.0.0.1 -p 45000 -res 1024x576 -map us -map us:dvorak -font 8 -term 1" &

---------------------------------------
ctrl+shift+` (left key from 1 above TAB) activate/deactivate terminal
ctrl+shift+z activate deactivate zoom
click on RESET_WINDOW_SIZE changes window size to current resolution only if not fullscreen or maximized
scroll wheel on IMAGE change image presets
scrool wheel on G,S,B,C changes gama,saturation,brightness,contrast
scroll wheel on MAX FPS changes maximum fps encoder will send
scroll wheel on FONT changes terminal font size
click on RES to set resolution
scroll wheel on RES change resolution
click on MAP set keyboard map
scroll wheel on MAP change keyboard mapping
click on ZOOM to activate/deactivate
right click ZOOM reset ZOOM to current resolution
scroll wheel on X,Y,W,H change zoom settings holding shift change by 10 holding ctrl by 50 holding shift+ctrl by 100
first row is stats from encoder
		C-cpu usage
		F-frames per second
		D-damage events per second
		K-kilobytes transfered from encoder per second
		c-time spent on image compare per second in miliseconds
		y-time spent converting argb to yuv per second in miliseconds
		x-time spent converting yuv to x264 compresed image per second in miliseconds
		s-time spent transfering compresed data to client per second in miliseconds
		u-percent of one second used for operations c+y+x+s
when xwingl not in focus encoder does not send data and xwingl is not drawing

options for encoder
	DISPLAY=:nn encoder -ip ipaddr -p port -xm Xvfb_framebuffers_file [ -yuv nnn ] [ -ct n ] [ -yt n ] [ -xt n ]
		DISPLAY=:nn :display where Xvfb is running\n");
		-yuv nnn :yuv format on start 420 or 444 can be changed from client
		-ct n :number of image compare threads 1-8 0-number of cpu's
		-yt n :number of argb to yuv threads 1-8 0-number of cpu's
		-xt n :number of x264 encoder threads 1-8 0-number of cpu's /2
example: DISPLAY=:15 encoder -ip 127.0.0.1 -p 33500 -xm /run/Xvfb_screen0 -yuv 444 -ct 1 -yt 1 -xt 1

options for xwingl
	xwingl -ip ipaddr -p port [ -res WxH ] [ -map xx[:yy] ] [ -img n.m,n.m,n.m,n.m ] [ -zoom x,y,w,h ] [ -cscale n ] [ -font n ] [ -term n ] [ -img_idx n ]
		-res WxH :resolution to add
		-map xx[:yy] :setxkbmap xx -variant yy
		-img n.m,n.m,n.m,n.m :gamma,saturation,brightness,contrast
		-zoom x,y,w,h :zoom part of screen
		-cscale n :cursor scale 1-8 0-2
		-font n :terminal font size 8,16,24,32,40 0-16
		-term n :terminal show on start 1 show 0 hide
		-img_idx n :default img index on start\n
example: xwingl -ip 127.0.0.1 -p 33500 -res 1024x576 -res 800x600 -map us -map us:dvorak -img 1.0,0.0,0.0,0.0 -img 1.0,0.2,-0.07,-0.12 -zoom 0,200,400,300 -cscale 1 -font 8 -term 1 -img_idx 1
