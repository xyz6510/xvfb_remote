
void draw_options()
{
	XWINGL_VAR *xwg=&xwingl_var;

	int i,j;
	for (i=1;i<terminal.h;i++) {
		for (j=0;j<terminal.w;j++) {
			term_print_at(j,i," ",0x0e06,0);
		}
	}
	if (xwg->srv_format==1) {
		term_print_at(0,1,"YUV420",0x0e06,0);
		term_print_at(7,1,"YUV444",0x060e,0);
	} else  if (xwg->srv_format==2) {
		term_print_at(0,1,"YUV420",0x060e,0);
		term_print_at(7,1,"YUV444",0x0e06,0);
	}
	term_print_at(14,1,"FULLSCREEN",0x0e06,0);
	term_print_at(25,1,"RESET_WINDOW_SIZE",0x0e06,0);
	char str[32];
	sprintf(str,"IMAGE:");term_print_at(0,2,str,0x0e06,0);
	if (xwg->image.num>0) {
		sprintf(str,"%0.2f %0.2f %0.2f %0.2f",xwg->image.g[xwg->img_idx],xwg->image.s[xwg->img_idx],
				xwg->image.b[xwg->img_idx],xwg->image.c[xwg->img_idx]);
		term_print_at(6,2,str,0x0e06,0);
	}
	sprintf(str,"G:%0.2f",video_var.screen_gamma);term_print_at(29,2,str,0x0e06,0);
	sprintf(str,"S:%0.2f",video_var.screen_saturation);term_print_at(36,2,str,0x0e06,0);
	sprintf(str,"B:%0.2f",video_var.screen_brightness);term_print_at(44,2,str,0x0e06,0);
	sprintf(str,"C:%0.2f",video_var.screen_contrast);term_print_at(52,2,str,0x0e06,0);
	sprintf(str,"MAX FPS:%i",xwg->srv_max_fps);term_print_at(0,3,str,0x0e06,0);
	sprintf(str,"FONT:%i",video_var.font_char_size);term_print_at(11,3,str,0x0e06,0);

	sprintf(str,"RES:");term_print_at(19,3,str,0x0e06,0);
	if (xwg->resolutions.num>0) {
		sprintf(str,"%ix%i",xwg->resolutions.resx[xwg->res_idx],xwg->resolutions.resy[xwg->res_idx]);
		term_print_at(23,3,str,0x0e06,0);
	}
	sprintf(str,"MAP:");term_print_at(33,3,str,0x0e06,0);
	if (xwg->xkmaps.num>0) {
		if (xwg->xkmaps.variant[xwg->map_idx]) {
			sprintf(str,"%s %s",xwg->xkmaps.map[xwg->map_idx],xwg->xkmaps.variant[xwg->map_idx]);
			term_print_at(37,3,str,0x0e06,0);
		} else if (xwg->xkmaps.map[xwg->map_idx]) {
			sprintf(str,"%s",xwg->xkmaps.map[xwg->map_idx]);
			term_print_at(37,3,str,0x0e06,0);
		}
	}

	if (xwg->zoom_active==1) {
		sprintf(str,"ZOOM");term_print_at(0,4,str,0x060e,0);
	} else {
		sprintf(str,"ZOOM");term_print_at(0,4,str,0x0e06,0);
	}
	sprintf(str,"X:%i",xwg->zoom_x);term_print_at(6,4,str,0x0e06,0);
	sprintf(str,"Y:%i",xwg->zoom_y);term_print_at(13,4,str,0x0e06,0);
	sprintf(str,"W:%i",xwg->zoom_w);term_print_at(20,4,str,0x0e06,0);
	sprintf(str,"H:%i",xwg->zoom_h);term_print_at(27,4,str,0x0e06,0);

}

void highlight_options(int becomes_active)
{
	XWINGL_VAR *xwg=&xwingl_var;

	int in_rect(int x,int y,int w,int h)
	{
		int fs=video_var.font_char_size;
		int mx=screen.mousebuf[10];
		int my=screen.mousebuf[11];
		x=terminal.x+(x*fs);
		int x1=terminal.x+x+(w*fs);
		y=terminal.y+(y*fs);
		int y1=terminal.y+y+(h*fs);
		int ret=0;
		if ((mx>x)&&(mx<x1)&&(my>y)&&(my<y1)) ret=1;
		return ret;
	}
	static int mb1_click=0;
	static int mb1_o=0;
	static int mb2_click=0;
	static int mb2_o=0;
	static int scroll_up=0;
	static int scroll_down=0;
	if (becomes_active) {
		scroll_up=screen.mousebuf[3];
		scroll_down=screen.mousebuf[4];
	}
	int is_mb1_click(int m1,int opt)
	{
		if (mb1_o==m1) return 0;
		mb1_o=m1;
		if ((m1>0)&&(mb1_click==0)) {
			mb1_click=opt;
			return 0;
		}
		if ((m1<0)&&(mb1_click==opt)) {
			mb1_click=0;
			return 1;
		}
		mb1_click=0;
		return 0;
	}
	int is_mb2_click(int m2,int opt)
	{
		if (mb2_o==m2) return 0;
		mb2_o=m2;
		if ((m2>0)&&(mb2_click==0)) {
			mb2_click=opt;
			return 0;
		}
		if ((m2<0)&&(mb2_click==opt)) {
			mb2_click=0;
			return 1;
		}
		mb2_click=0;
		return 0;
	}
	int is_scroll(int up,int down)
	{
		int ret=0;
		if (scroll_up!=up) {
			scroll_up=up;
			ret=1;
		}
		if (scroll_down!=down) {
			scroll_down=down;
			ret=-1;
		}
		return ret;
	}
	void inc_dec_value(int dir,float *val,float min,float max,float v)
	{
		if (dir<0) v=-v;
		float fl=*val;
		fl=fl+v;
		if (max<min) max=min;
		if (min>max) min=max;
		if (fl<min) fl=min;
		if (fl>max) fl=max;
		if (fabs(fl)<0.0002) fl=0.0f;
		//printf("fl:%0.2f\n",fl);
		*val=fl;
	}
	void inc_dec_value_int(int dir,int *val,int min,int max,int v)
	{
		if (dir<0) v=-v;
		int fl=*val;
		fl=fl+v;
		if (max<min) max=min;
		if (min>max) min=max;
		if (fl<min) fl=min;
		if (fl>max) fl=max;
		//printf("fl:%i\n",fl);
		*val=fl;
	}
	if (xwg->terminal_inside) {
		int mb1=screen.mousebuf[0];
		int mb2=screen.mousebuf[1];
		int ctrl=screen.keybuf[37];
		int shift=screen.keybuf[50];
		draw_options();
		char str[32];
		if (in_rect(0,1,6,1)) {
			sprintf(str,"YUV420");term_print_at(0,1,str,0x060e,0);
			if (is_mb1_click(mb1,1)==1) flip(&xwg->command_set_yuv,1);
		}
		if (in_rect(7,1,6,1)) {
			sprintf(str,"YUV444");term_print_at(7,1,str,0x060e,0);
			if (is_mb1_click(mb1,2)==1) flip(&xwg->command_set_yuv,-1);
		}
		if (in_rect(14,1,10,1)) {
			sprintf(str,"FULLSCREEN");term_print_at(14,1,str,0x060e,0);
			if (is_mb1_click(mb1,3)==1) {
				static int full=0;
				full^=1;
				set_fullscreen(full,xwg->win_resizable);
				//printf("fullscreen click\n");
			}
		}
		if (in_rect(25,1,17,1)) {
			sprintf(str,"RESET_WINDOW_SIZE");term_print_at(25,1,str,0x060e,0);
			if (is_mb1_click(mb1,4)==1) {
				set_window_size(xwg->srv_w,xwg->srv_h,xwg->win_resizable);
				resize_framebuffer(screen.ww,screen.wh);
				//printf("reset_window_size click\n");
			}
		}
		if (in_rect(0,2,25,1)) {
			sprintf(str,"IMAGE:");term_print_at(0,2,str,0x060e,0);
			if (xwg->image.num>0) {
				sprintf(str,"%0.2f %0.2f %0.2f %0.2f",xwg->image.g[xwg->img_idx],xwg->image.s[xwg->img_idx],
					xwg->image.b[xwg->img_idx],xwg->image.c[xwg->img_idx]);
				term_print_at(6,2,str,0x060e,0);
			}
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				if (xwg->image.num>0) {
					inc_dec_value_int(s,&xwg->img_idx,0,xwg->image.num-1,1);
					video_var.screen_gamma		=xwg->image.g[xwg->img_idx];
					video_var.screen_saturation	=xwg->image.s[xwg->img_idx];
					video_var.screen_brightness	=xwg->image.b[xwg->img_idx];
					video_var.screen_contrast	=xwg->image.c[xwg->img_idx];
				}
			}
		}
		if (in_rect(29,2,6,1)) {
			sprintf(str,"G:%0.2f",video_var.screen_gamma);term_print_at(29,2,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) inc_dec_value(s,&video_var.screen_gamma,0.0,2.0,0.1);
		}
		if (in_rect(36,2,6,1)) {
			sprintf(str,"S:%0.2f",video_var.screen_saturation);term_print_at(36,2,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) inc_dec_value(s,&video_var.screen_saturation,-1.0,1.0,0.05);
		}
		if (in_rect(44,2,6,1)) {
			sprintf(str,"B:%0.2f",video_var.screen_brightness);term_print_at(44,2,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) inc_dec_value(s,&video_var.screen_brightness,-1.0,1.0,0.01);
		}
		if (in_rect(52,2,6,1)) {
			sprintf(str,"C:%0.2f",video_var.screen_contrast);term_print_at(52,2,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) inc_dec_value(s,&video_var.screen_contrast,-1.0,1.0,0.01);
		}
		if (in_rect(0,3,10,1)) {
			sprintf(str,"MAX FPS:%i",xwg->srv_max_fps);term_print_at(0,3,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				int v=xwg->srv_max_fps;
				inc_dec_value_int(s,&v,5,60,5);
				if (v!=xwg->srv_max_fps) {
					xwg->srv_max_fps=v;
					xwg->command_set_max_fps=1;
				}
			}
		}
		if (in_rect(11,3,7,1)) {
			sprintf(str,"FONT:%i",video_var.font_char_size);term_print_at(11,3,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				int v=video_var.font_char_size;
				if (s>0) v+=8;
				if (s<0) v-=8;
				if (v>40) v=40;
				if (v<8) v=8;
				if (v==8) video_var.font_alpha_cut=0.5;
				if (v==16) video_var.font_alpha_cut=0.6;
				if (v==24) video_var.font_alpha_cut=0.5;
				if (v==32) video_var.font_alpha_cut=0.6;
				if (v==40) video_var.font_alpha_cut=0.5;
				video_var.font_char_size=v;
			}
		}
		if (in_rect(19,3,11,1)) {
			sprintf(str,"RES:");term_print_at(19,3,str,0x060e,0);
			if (xwg->resolutions.num>0) {
				sprintf(str,"%ix%i",xwg->resolutions.resx[xwg->res_idx],xwg->resolutions.resy[xwg->res_idx]);
				term_print_at(23,3,str,0x060e,0);
			}
			if (is_mb1_click(mb1,6)==1) xwg->command_set_resolution=1;
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) inc_dec_value_int(s,&xwg->res_idx,0,xwg->resolutions.num-1,1);
		}
		if (in_rect(33,3,4,1)) {
			sprintf(str,"MAP:");term_print_at(33,3,str,0x060e,0);
			if (xwg->xkmaps.num>0) {
				if (xwg->xkmaps.variant[xwg->map_idx]) {
					sprintf(str,"%s %s",xwg->xkmaps.map[xwg->map_idx],xwg->xkmaps.variant[xwg->map_idx]);
					term_print_at(37,3,str,0x060e,0);
				} else if (xwg->xkmaps.map[xwg->map_idx]) {
					sprintf(str,"%s",xwg->xkmaps.map[xwg->map_idx]);
					term_print_at(37,3,str,0x060e,0);
				}
			}
			if (is_mb1_click(mb1,7)==1) xwg->command_set_xkmap=1;
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) inc_dec_value_int(s,&xwg->map_idx,0,xwg->xkmaps.num-1,1);
		}
		if (in_rect(0,4,4,1)) {
			sprintf(str,"ZOOM");term_print_at(0,4,str,0x060e,0);
			if (is_mb1_click(mb1,5)==1) {
				xwg->zoom_active^=1;
				video_var.zoom=xwg->zoom_active;
				if (xwg->zoom_active==1) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
				if (xwg->zoom_active==0) set_rescale_screen(0,0,xwg->srv_w,xwg->srv_h);
				resize_framebuffer(screen.ww,screen.wh);
			}
			if (is_mb2_click(mb2,5)==1) {
				xwg->zoom_x=0;
				xwg->zoom_y=0;
				xwg->zoom_w=xwg->srv_w;
				xwg->zoom_h=xwg->srv_h;
				set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			}
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				if (xwg->zoom.num>0) {
					inc_dec_value_int(s,&xwg->zoom_idx,0,xwg->zoom.num-1,1);
					int zx=xwg->zoom.x[xwg->zoom_idx];
					int zy=xwg->zoom.y[xwg->zoom_idx];
					int zw=xwg->zoom.w[xwg->zoom_idx];
					int zh=xwg->zoom.h[xwg->zoom_idx];
					inc_dec_value_int(s,&zx,0,xwg->srv_w-1,0);
					inc_dec_value_int(s,&zy,0,xwg->srv_h-1,0);
					inc_dec_value_int(s,&zw,1,xwg->srv_w,0);
					inc_dec_value_int(s,&zh,1,xwg->srv_h,0);
					if (zx+zw>xwg->srv_w) zw=xwg->srv_w-zx;
					if (zy+zh>xwg->srv_h) zh=xwg->srv_h-zy;
					xwg->zoom_x=zx;xwg->zoom_y=zy;
					xwg->zoom_w=zw;xwg->zoom_h=zh;
					if (xwg->zoom_active) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
				}
			}
		}
		int v=1;
		if (shift>0) v=10;
		if (ctrl>0) v=50;
		if ((shift>0)&&(ctrl>0)) v=100;
		if (in_rect(6,4,3,1)) {
			sprintf(str,"X:%i",xwg->zoom_x);term_print_at(6,4,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				inc_dec_value_int(s,&xwg->zoom_x,0,xwg->srv_w-1,v);
				if (xwg->zoom_x+xwg->zoom_w>xwg->srv_w) {
					xwg->zoom_w=xwg->srv_w-xwg->zoom_x;
				}
				if (xwg->zoom_active) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			}
		}
		if (in_rect(13,4,3,1)) {
			sprintf(str,"Y:%i",xwg->zoom_y);term_print_at(13,4,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				inc_dec_value_int(s,&xwg->zoom_y,0,xwg->srv_h-1,v);
				if (xwg->zoom_y+xwg->zoom_h>xwg->srv_h) {
					xwg->zoom_h=xwg->srv_h-xwg->zoom_y;
				}
				if (xwg->zoom_active) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			}
		}
		if (in_rect(20,4,3,1)) {
			sprintf(str,"W:%i",xwg->zoom_w);term_print_at(20,4,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				inc_dec_value_int(s,&xwg->zoom_w,1,xwg->srv_w,v);
				if (xwg->zoom_x+xwg->zoom_w>xwg->srv_w) {
					xwg->zoom_w=xwg->srv_w-xwg->zoom_x;
				}
				if (xwg->zoom_active) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			}
		}
		if (in_rect(27,4,3,1)) {
			sprintf(str,"H:%i",xwg->zoom_h);term_print_at(27,4,str,0x060e,0);
			int s=is_scroll(screen.mousebuf[3],screen.mousebuf[4]);
			if (s) {
				inc_dec_value_int(s,&xwg->zoom_h,1,xwg->srv_h,v);
				if (xwg->zoom_y+xwg->zoom_h>xwg->srv_h) {
					xwg->zoom_h=xwg->srv_h-xwg->zoom_y;
				}
				if (xwg->zoom_active) set_rescale_screen(xwg->zoom_x,xwg->zoom_y,xwg->zoom_w,xwg->zoom_h);
			}
		}
		scroll_up=screen.mousebuf[3];
		scroll_down=screen.mousebuf[4];
	} else {
		mb1_click=0;
		mb2_click=0;
		scroll_up=screen.mousebuf[3];
		scroll_down=screen.mousebuf[4];
		draw_options();
	}
	xwg->terminal_update=1;
}
