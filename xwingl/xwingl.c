#include "types.h"

#include "x11/x11.c"
#include "ogl/ogl.c"
#include "term/term.c"

#include "socket.c"
#include "parse_cmd.c"
#include "command.c"
#include "stats.c"
#include "input.c"
#include "x264.c"
#include "options.c"
#include "cursor.c"
#include "clipboard_change.c"
#include "typed_events.c"
#include "get_clipboard.c"
#include "send_clipboard.c"

int main(int argc,char **argv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	memset(xwg,0,sizeof(XWINGL_VAR));

	signal(SIGPIPE,SIG_IGN);

	parse_cmd(argc,argv);

	xwg->clip_data=malloc(64*1024);

	xwg->conn_x264=connect_to("x264",xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num);

	read_data(xwg->conn_x264,&xwg->srv_fbw,4,NULL);
	read_data(xwg->conn_x264,&xwg->srv_fbh,4,NULL);
	read_data(xwg->conn_x264,&xwg->srv_w,4,NULL);
	read_data(xwg->conn_x264,&xwg->srv_h,4,NULL);
	read_data(xwg->conn_x264,&xwg->srv_format,4,NULL);
	read_data(xwg->conn_x264,&xwg->srv_max_fps,4,NULL);
	printf("server fb:%ix%i res:%ix%i\n",xwg->srv_fbw,xwg->srv_fbh,xwg->srv_w,xwg->srv_h);

	xwg->zoom_w=xwg->srv_w;
	xwg->zoom_h=xwg->srv_h;

	xwg->win_resizable=1;
	int borderless=0;
	int winx=xwg->srv_w,winy=xwg->srv_h;
	open_display("xwingl",xwg->win_resizable,borderless);
	set_window_size(winx,winy,xwg->win_resizable);
	while ((screen.ww!=winx)||(screen.wh!=winy)) usleep(1000);
	set_window_pos(-1,-1,xwg->win_resizable);

	int deskx,desky;
	get_root_size(&deskx,&desky);
	init_opengl(winx,winy,deskx,desky,xwg->srv_fbw,xwg->srv_fbh,xwg->srv_w,xwg->srv_h,xwg->srv_format);

	init_terminal();

	terminal.h=5;
	terminal.w=64;
	terminal.cfgbg=0x2020;
	video_var.font_char_size=16;
	video_var.font_alpha_cut=0.6;
	if (xwg->font_size) video_var.font_char_size=xwg->font_size;

	if (xwg->img_idx<xwg->image.num) {
		video_var.screen_gamma		=xwg->image.g[xwg->img_idx];
		video_var.screen_saturation	=xwg->image.s[xwg->img_idx];
		video_var.screen_brightness	=xwg->image.b[xwg->img_idx];
		video_var.screen_contrast	=xwg->image.c[xwg->img_idx];

	} else xwg->img_idx=0;

	draw_options();

	if (xwg->term_active) term_show(1);
	else term_show(0);

	xwg->UTF8=XInternAtom(screen.d,"UTF8_STRING",True);
	xwg->UTF8_1=XInternAtom(screen.d,"text/plain;charset=utf-8",True);
	xwg->TARGETS=XInternAtom(screen.d,"TARGETS",False);

	pthread_attr_t process_attr;
	pthread_attr_init(&process_attr);
	pthread_attr_setstacksize(&process_attr,64*1024);

	pthread_t x264_thread;
	pthread_create(&x264_thread,&process_attr,(void*)x264,NULL);

	SRV input_data={"input",1};
	pthread_t input_thread;
	pthread_create(&input_thread,&process_attr,(void*)input,&input_data);

	SRV stats_data={"stats",2};
	pthread_t stats_thread;
	pthread_create(&stats_thread,&process_attr,(void*)stats,&stats_data);

	SRV command_data={"command",3};
	pthread_t command_thread;
	pthread_create(&command_thread,&process_attr,(void*)command,&command_data);

	SRV cursor_data={"cursor",4};
	pthread_t cursor_thread;
	pthread_create(&cursor_thread,&process_attr,(void*)cursor,&cursor_data);

	pthread_t clipboard_change_thread;
	pthread_create(&clipboard_change_thread,&process_attr,(void*)clipboard_change,NULL);

	SRV clip_get_data={"clipboard get",5};
	pthread_t get_clipboard_thread;
	pthread_create(&get_clipboard_thread,&process_attr,(void*)get_clipboard,&clip_get_data);

	SRV clip_send_data={"clipboard send",6};
	pthread_t send_clipboard_thread;
	pthread_create(&send_clipboard_thread,&process_attr,(void*)send_clipboard,&clip_send_data);

	pthread_t typed_event_thread;
	pthread_create(&typed_event_thread,&process_attr,(void*)typed_events,NULL);

	int *keybuf=screen.keybuf;
	int *mousebuf=screen.mousebuf;
	int kbuf[256];
	int mbuf[256];
	int i;
	for (i=0;i<256;i++) {
		kbuf[i]=0;
		mbuf[i]=0;
	}
	int check_key(int k,int ctrl,int shift)
	{
		int ret=0;
		int c=keybuf[37];
		int s=keybuf[50];
		if (ctrl==0) c=1;
		if (shift==0) s=1;
		if (kbuf[k]!=keybuf[k]) {
			kbuf[k]=keybuf[k];
			if ((kbuf[k])&&(c>0)&&(s>0)) {
				if (kbuf[k]>0) ret=1;
			}
		}
		return ret;
	}
	int check_mouse(int m)
	{
		int ret=0;
		if (mousebuf[m]!=mbuf[m]) {
			mbuf[m]=mousebuf[m];
			ret=1;
		}
		return ret;
	}

	while((xwg->end==0)&&(screen.end==0)) {
		if (xwg->reconfigure_screen==1) {
			xwg->reconfigure_screen=0;
			video_var.res_x=xwg->srv_w;
			video_var.res_y=xwg->srv_h;
			set_yuv_format(xwg->srv_format);
			xwg->zoom_active=0;
			xwg->zoom_x=0;
			xwg->zoom_y=0;
			xwg->zoom_w=xwg->srv_w;
			xwg->zoom_h=xwg->srv_h;
			video_var.zoom=xwg->zoom_active;
			set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			resize_framebuffer(screen.ww,screen.wh);
		}
		if (xwg->terminal_inside==0) {
			if ((check_mouse(3)==1)||(check_mouse(4)==1)) {// scroll
				xwg->command_limit_fps=1;
			}
		}
		if (terminal.active==1) {
			int rx,ry,rx1,ry1,mx,my;
			mx=mousebuf[10];
			my=mousebuf[11];
			rx=terminal.x;
			ry=terminal.y;
			rx1=rx+terminal.w*video_var.font_char_size;
			ry1=ry+terminal.h*video_var.font_char_size;
			int becomes_active=0;
			if ((mx>=rx)&&(mx<=rx1)&&(my>=ry)&&(my<=ry1)) {
				if (xwg->terminal_inside==0) {
					XUndefineCursor(screen.d,screen.w);
					xwg->release_mouse_buttons=1;
					becomes_active=1;
				}
				xwg->terminal_inside=1;
			} else {
				if (xwg->terminal_inside==1) {
					XDefineCursor(screen.d,screen.w,xwg->win_cursor);
					xwg->release_mouse_buttons=1;
				}
				xwg->terminal_inside=0;
			}
			highlight_options(becomes_active);
		}
		if (check_key(49,1,1)) {// left key from 1
			term_show(-1);
			if (terminal.active==0) {
				if (xwg->terminal_inside==1) XDefineCursor(screen.d,screen.w,xwg->win_cursor);
				xwg->terminal_inside=0;
			}
			xwg->release_mouse_buttons=1;
		}
		if (check_key(52,1,1)) {// z
			xwg->zoom_active^=1;
			video_var.zoom=xwg->zoom_active;
			if (xwg->zoom_active==1) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			if (xwg->zoom_active==0) set_rescale_screen(0,0,xwg->srv_w,xwg->srv_h);
			resize_framebuffer(screen.ww,screen.wh);
		}
		if (screen.ev.res) {
			screen.ev.res=0;
			resize_framebuffer(screen.ww,screen.wh);
		}
		if ((screen.ev.map==1)&&(screen.ev.focus)==1) {
			if (xwg->terminal_update==1) {
				xwg->terminal_update=0;
				term_update();
			}
			flip(&xwg->command_active_encoder,1);
			ogl_draw();
		} else {
			flip(&xwg->command_active_encoder,-1);
			usleep(40000);
		}
	}

	xwg->end=1;

	close_socket(xwg->conn_x264);
	close_socket(xwg->conn_input);
	close_socket(xwg->conn_stats);
	close_socket(xwg->conn_cursor);
	close_socket(xwg->conn_command);
	close_socket(xwg->conn_get_clipboard);
	close_socket(xwg->conn_send_clipboard);

	close_display();

	pthread_join(x264_thread,NULL);
	pthread_join(input_thread,NULL);
	pthread_join(stats_thread,NULL);
	pthread_join(command_thread,NULL);
	pthread_join(cursor_thread,NULL);
	pthread_join(get_clipboard_thread,NULL);
	pthread_join(send_clipboard_thread,NULL);

	free(xwg->clip_data);

	return 0;
}
