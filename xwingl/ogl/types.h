
#include <math.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdint.h>

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

typedef struct {
	double time;
	double dtime;
	double rtime;
	double wtime;
	int usleep;

	int screen_x;
	int screen_y;

	int img_x;
	int img_y;

	int res_x;
	int res_y;

	int desktop_x;
	int desktop_y;

	int zoom;

	int act_screen;
	int act_y;
	int act_u;
	int act_v;
	int act_font;
	int act_font_rgba16i;
	int act_font_pal;
	int act_rescale;

	GLfloat projection[16];
	GLfloat font_projection[16];

	float screen_saturation;//-1,1
	float screen_contrast;//-1,1
	float screen_brightness;//-1,1
	float screen_gamma;//0,10

	int yuv_update;
	int yuv_format;

	int font_char_size;
	float font_alpha_cut;

	int idx_tex_screen;
	int idx_linear;
	int idx_screen;
	int idx_saturation;
	int idx_contrast;
	int idx_brightness;
	int idx_gamma;

	int idx_yuv_screen;
	int idx_yuv_format;//1=yuv444 2=yuv420

	int idx_rescale_screen;

	int idx_font_screen;
	int idx_font_projection;
	int idx_font_char_size;
	int idx_font_alpha_cut;

	GLuint screen_tex;
	GLuint y_tex;
	GLuint u_tex;
	GLuint v_tex;
	GLuint font_tex;
	GLuint font_tex_rgba16i;
	GLuint font_tex_pal;
	GLuint rescale_tex;

	GLuint fb_screen;
	GLuint fb_rescale;

	GLuint shd_screen;
	GLuint shd_yuv;
	GLuint shd_font;
	GLuint shd_rescale;

	GLuint va0;
	GLuint va1;

	uint8_t *y_data;
	uint8_t *u_data;
	uint8_t *v_data;

	int y_line_size;
	int u_line_size;
	int v_line_size;

	void *font_data_buf;
	int font_start_char;
	int font_end_char;
} VIDEO_VAR;

VIDEO_VAR video_var;

