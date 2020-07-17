#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include </usr/include/X11/XWDFile.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/extensions/XTest.h>

#include <libyuv.h>
#include <x264.h>

#include <smmintrin.h>

#include <sys/resource.h>

typedef struct {
	Display *d;
	Window root;
	Window win;
	int cpu;
	int w;
	int h;
	int fbw;
	int fbh;
	int row;
	int format;
	int map_size;
	void *xvfb;
	void *img;
	int dmg_cnt;
	int stat_dmg;
	int stat_dmg_cnt2;
	int stat_fps;
	int stat_fps_cnt;
	int stat_data_send;
	int stat_data_send_kb;
	int stat_cpu_usage;
	int stat_cmp_time;
	int stat_yuv_time;
	int stat_x264_time;
	int stat_send_time;
	uint8_t *clip_data;
	int clip_size;

	int end;
	int command_active_encoder;
	int command_set_yuv420;
	int command_set_yuv444;
	int command_limit_fps;
	int command_set_max_fps;
	int command_set_resolution;
	int command_set_xkmap;
	int limit_fps_def;
	int send_clipboard_data;
	Atom UTF8;
	Atom UTF8_1;
	Atom TARGETS;
	int ip_addr_num;
	int ip_port_num;
	char *ip_addr_str;
	char *ip_port_str;
	char *xvfb_file;

	int x264_threads;
	int cmp_threads;
	int yuv_threads;
} SCR;

typedef struct {
	x264_param_t param;
	x264_t* encoder;
	x264_picture_t in;
	x264_picture_t out;
	x264_nal_t* nals;
	int i_nals;
} X264;

typedef struct {
	char *str;
	int p;
} SRV;

SCR screen;

X264 x264;
