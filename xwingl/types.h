#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>

#include <libavcodec/avcodec.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

typedef struct {
	int num;
	int max;
	int *resx;
	int *resy;
} RES;

typedef struct {
	int num;
	int max;
	char **map;
	char **variant;
} XKMAP;

typedef struct {
	int num;
	int max;
	float *g;
	float *s;
	float *b;
	float *c;
} IMAGE;

typedef struct {
	int num;
	int max;
	int *x;
	int *y;
	int *w;
	int *h;
} ZOOM;



typedef struct {
	int end;
	int conn_x264;
	int conn_input;
	int conn_stats;
	int conn_cursor;
	int conn_command;
	int conn_get_clipboard;
	int conn_send_clipboard;
	int srv_fbw;
	int srv_fbh;
	int srv_w;
	int srv_h;
	int srv_format;
	int srv_max_fps;
	int terminal_update;
	int terminal_inside;
	int reconfigure_screen;
	int command_active_encoder;
	int command_set_yuv;
	int command_limit_fps;
	int command_set_max_fps;
	int command_set_resolution;
	int command_set_xkmap;
	int win_resizable;
	Cursor win_cursor;
	int cursor_scale;
	int release_mouse_buttons;
	uint8_t *clip_data;
	int clip_size;
	Atom UTF8;
	Atom UTF8_1;
	Atom TARGETS;
	int zoom_active;
	int zoom_x;
	int zoom_y;
	int zoom_w;
	int zoom_h;

	int send_clipboard_data;

	int ip_addr_num;
	int ip_port_num;
	char *ip_addr_str;
	char *ip_port_str;

	RES resolutions;

	XKMAP xkmaps;

	IMAGE image;

	ZOOM zoom;

	int res_idx;
	int map_idx;
	int img_idx;
	int zoom_idx;

	int font_size;
	int term_active;

} XWINGL_VAR;

typedef struct {
	char *str;
	int p;
} SRV;

XWINGL_VAR xwingl_var;
