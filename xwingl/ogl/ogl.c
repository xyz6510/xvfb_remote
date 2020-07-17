
#include "types.h"

#include "font_x20_xcf.c"
#include "glerr.c"
#include "print_gl_info.c"
#include "compile_shader.c"
#include "init_texture.c"
#include "update_texture.c"
#include "ortho2d.c"
#include "init_gl.c"
#include "draw_yuv.c"
#include "draw_rescale.c"
#include "draw_fb_screen.c"
#include "draw_font.c"
#include "draw.c"
#include "resize_framebuffer.c"
#include "set_yuv_format.c"
#include "set_rescale_screen.c"

void init_opengl(int x,int y,int dx,int dy,int fbx,int fby,int resx,int resy,int format)
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	glXSwapBuffers(screen.d,screen.w);

	init_gl(x,y,dx,dy,fbx,fby,resx,resy,format);
}

void ogl_draw()
{
	struct timeval t1,t2;
	const double fps_60=(1000*1000)/60;
	gettimeofday(&t1,NULL);

	draw();

	unsigned int vsync;
	glXWaitVideoSyncSGI(1,0,&vsync);
	glXSwapBuffers(screen.d,screen.w);

	gettimeofday(&t2,NULL);
	double td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);

	VIDEO_VAR *v=&video_var;

	v->rtime=td;

	v->usleep=0;
	if (td<4000) {
		int wait=fps_60-td;
		usleep(wait);
		v->usleep=1;
	}

	gettimeofday(&t2,NULL);
	td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);

	v->wtime=td;

	if (td==0) v->dtime=1;
	else v->dtime=(float)fps_60/td;
	if (v->dtime>2) v->dtime=2;
	v->dtime=round(v->dtime*100.0)/100.0;
	v->time=(t2.tv_sec*1000000)+t2.tv_usec;
}
