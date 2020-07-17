
void create_chars(int y,int x,int w,int h,int row,int bit,int bs,void *font,int num_chars,void *adr,int pad)
{
	uint8_t  (*ascii8)[h]=(uint8_t(*)[h]) font;
	uint16_t (*ascii16)[h]=(uint16_t(*)[h]) font;
	unsigned int (*buf)[row]=(unsigned int(*)[row]) adr;
	int i;
	int x1=x+pad;
	int y1=y+pad;
	for (i=0;i<num_chars;i++) {
		int j;
		for (j=0;j<h;j++) {
			int k;
			for (k=0;k<w;k++) {
				int act=bit>>k;
				if (bs==8) act= ascii8[i][j]&act;
				if (bs==16)act=ascii16[i][j]&act;
				buf[y1+j][x1+k]=0;
				if (act) buf[y1+j][x1+k]=0xff000000;
			}
		}
		x1+=w+pad;
		if (x1>=row) {
			x1=x+pad;
			y1+=h+pad;
		}
	}
}

void set_default_pal()
{
	VIDEO_VAR *v=&video_var;

	unsigned int pal[16]={ // a b g r texture this a r g b
		0xff000000,
		0xffffffff,
		0xffd9311a,
		0xff87d4db,
		0xffa05abf,
		0xff71be43,
		0xff4242e7,
		0xffffff00,
		0xffff8835,
		0xff624900,
		0xffffa0a0,
		0xff545454,
		0xff6c6c6c,
		0xff98ff5a,
		0xffa5a5ff,
		0xff959595,
	};
	int *mem=malloc(2*64*4);
	memset(mem,0,2*64*4);
	int *p0=mem;
	int *p1=&mem[64];
	char *cl0,*cl1;;
	int i;
	for (i=0;i<16;i++) {
		p0[i]=pal[i];
		cl0=(char*)&p0[i];
		cl1=(char*)&pal[i];
		cl0[0]=cl1[2];
		cl0[2]=cl1[0];
		p1[i]=pal[i];
		cl0=(char*)&p1[i];
		cl0[0]=cl1[2];
		cl0[2]=cl1[0];
	}
	for (i=0;i<16;i++) {
		p0[16+i]=p0[i];
		cl0=(char*)&p0[16+i];
		cl0[3]=0x7f;
		p1[16+i]=p1[i];
		cl0=(char*)&p1[16+i];
		cl0[3]=0x7f;
	}
	p0[32]=0;
	p1[32]=0;

	update_texture(v->act_font_pal,v->font_tex_pal,0,0,64,2,0,GL_RGBA,GL_UNSIGNED_BYTE,mem);
	free(mem);
}

void init_gl(int screen_x,int screen_y,int desktop_x,int desktop_y,int fbx,int fby,int resx,int resy,int format)
{
	VIDEO_VAR *v=&video_var;

	struct timeval time;
	gettimeofday(&time,NULL);
	v->time=(time.tv_sec*1000000)+time.tv_usec;
	v->dtime=1;

	v->screen_x=screen_x;
	v->screen_y=screen_y;

	v->img_x=fbx;//screen_x;
	v->img_y=fby;//screen_y;

	v->res_x=resx;//screen_x;
	v->res_y=resy;//screen_y;

	v->zoom=0;

	v->act_screen=0;
	v->act_y=1;
	v->act_u=2;
	v->act_v=3;
	v->act_rescale=4;
	v->act_font=5;
	v->act_font_rgba16i=6;
	v->act_font_pal=7;

	v->font_char_size=8;
	v->font_alpha_cut=0.5;

	v->yuv_format=format;

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glGenTextures(1,&v->screen_tex);
	glGenTextures(1,&v->y_tex);
	glGenTextures(1,&v->u_tex);
	glGenTextures(1,&v->v_tex);
	glGenTextures(1,&v->font_tex);
	glGenTextures(1,&v->font_tex_rgba16i);
	glGenTextures(1,&v->font_tex_pal);
	glGenTextures(1,&v->rescale_tex);

	//fb_screen=xvfb_fb
	//yuv xvfb_fb 444
	//change cordinates depending of encoder res
	glGenFramebuffers(1,&v->fb_screen);
	init_texture(v->act_screen,v->img_x,v->img_y,v->screen_tex,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE,0,1,0);

	init_texture(v->act_y,v->img_x,v->img_y,v->y_tex,GL_R8,GL_RED,GL_UNSIGNED_BYTE,1,0,0);
	init_texture(v->act_u,v->img_x,v->img_y,v->u_tex,GL_R8,GL_RED,GL_UNSIGNED_BYTE,1,0,0);
	init_texture(v->act_v,v->img_x,v->img_y,v->v_tex,GL_R8,GL_RED,GL_UNSIGNED_BYTE,1,0,0);

	int pad=2;
	init_texture(v->act_font,128+17*2,88+12*2,v->font_tex,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE,0,1,1);
	int msize=(128+17*pad)*(88+12*pad)*4;
	char *mem=malloc(msize);
	memset(mem,0,msize);
	create_chars(0,0,8,8,128+17*pad,0b10000000,8,fonts_x20_xcf,176,mem,pad);
	update_texture(v->act_font,v->font_tex,0,0,128+17*2,88+12*2,0,GL_RGBA,GL_UNSIGNED_BYTE,mem);
	//int f=open("/run/fnt.data",O_RDWR|O_CREAT|O_TRUNC,0644);
	//write(f,mem,msize);
	//close(f);
	free(mem);

	init_texture(v->act_font_rgba16i,256,256,v->font_tex_rgba16i,GL_RGBA16I,GL_RGBA_INTEGER,GL_SHORT,0,0,0);
	init_texture(v->act_font_pal,64,2,v->font_tex_pal,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE,0,0,0);

	set_default_pal();

	glGenFramebuffers(1,&v->fb_rescale);
	v->desktop_x=desktop_x;
	v->desktop_y=desktop_y;
	init_texture(v->act_rescale,v->desktop_x,v->desktop_y,v->rescale_tex,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE,0,0,0);

	glBindFramebuffer(GL_FRAMEBUFFER,v->fb_screen);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,v->screen_tex,0);

	glBindFramebuffer(GL_FRAMEBUFFER,v->fb_rescale);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,v->rescale_tex,0);

	glBindFramebuffer(GL_FRAMEBUFFER,0);

	ortho2d(0,0,1,1,v->projection,0);

	ortho2d(0,0,v->screen_x,v->screen_y,v->font_projection,1);

	//-- v->va0 --
	glGenVertexArrays(1,&v->va0);
	glBindVertexArray(v->va0);
	int size_vbo=1*1*(6*2)*4;
	float *data_vbo=malloc(size_vbo);
	memset(data_vbo,0,size_vbo);
	data_vbo[0] =0;data_vbo[1] =0;
	data_vbo[2] =1;data_vbo[3] =0;
	data_vbo[4] =1;data_vbo[5] =1;
	data_vbo[6] =1;data_vbo[7] =1;
	data_vbo[8] =0;data_vbo[9] =1;
	data_vbo[10]=0;data_vbo[11]=0;

	GLuint vbo;
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,size_vbo,data_vbo,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
	free(data_vbo);
	//----------------

	//-- v->va1 --
	glGenVertexArrays(1,&v->va1);
	glBindVertexArray(v->va1);
	size_vbo=256*256*(6*4)*4;
	data_vbo=malloc(size_vbo);
	memset(data_vbo,0,size_vbo);
	int m=0,i,j;
	for (i=0;i<256;i++) {
		for (j=0;j<256;j++) {
			data_vbo[m+0] =0;data_vbo[m+1] =0;data_vbo[m+2] =j;data_vbo[m+3] =i;
			data_vbo[m+4] =1;data_vbo[m+5] =0;data_vbo[m+6] =j;data_vbo[m+7] =i;
			data_vbo[m+8] =1;data_vbo[m+9] =1;data_vbo[m+10]=j;data_vbo[m+11]=i;
			data_vbo[m+12]=1;data_vbo[m+13]=1;data_vbo[m+14]=j;data_vbo[m+15]=i;
			data_vbo[m+16]=0;data_vbo[m+17]=1;data_vbo[m+18]=j;data_vbo[m+19]=i;
			data_vbo[m+20]=0;data_vbo[m+21]=0;data_vbo[m+22]=j;data_vbo[m+23]=i;
			m+=24;
		}
	}

	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,size_vbo,data_vbo,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,NULL);
	free(data_vbo);
	//------------------


	//-- v->shd_screen --
	{
	v->shd_screen=glCreateProgram();
	GLuint vs=compile_shader("ogl/shader/vs_screen.vert",GL_VERTEX_SHADER);
	GLuint fs=compile_shader("ogl/shader/fs_screen.frag",GL_FRAGMENT_SHADER);
	glAttachShader(v->shd_screen,vs);
	glAttachShader(v->shd_screen,fs);
    glLinkProgram(v->shd_screen);
    print_link_log("shd_screen",v->shd_screen);

	glUseProgram(v->shd_screen);

	int i;
	i=glGetUniformLocation(v->shd_screen,"projection");glUniformMatrix4fv(i,1,GL_FALSE,v->projection);
	i=v->idx_tex_screen=glGetUniformLocation(v->shd_screen,"tex_screen");glUniform1i(i,v->act_screen);
	v->idx_linear=glGetUniformLocation(    v->shd_screen,"linear"); glUniform1f(v->idx_linear,1);
	i=glGetUniformLocation(    			   v->shd_screen,"fb_size");glUniform2f(i,v->img_x,v->img_y);
	v->idx_screen=glGetUniformLocation(    v->shd_screen,"screen"); glUniform2f(v->idx_screen,v->res_x,v->res_y);
	i=glGetUniformLocation(    			   v->shd_screen,"desktop");glUniform2f(i,v->desktop_x,v->desktop_y);
	v->idx_saturation=glGetUniformLocation(v->shd_screen,"saturation");
	v->idx_contrast=glGetUniformLocation(  v->shd_screen,"contrast");
	v->idx_brightness=glGetUniformLocation( v->shd_screen,"brightness");
	v->idx_gamma=glGetUniformLocation(     v->shd_screen,"gamma");
	}
	//-----------------

	//-- v->shd_yuv --
	{
	v->shd_yuv=glCreateProgram();
	GLuint vs=compile_shader("ogl/shader/vs_yuv.vert",GL_VERTEX_SHADER);
	GLuint fs=compile_shader("ogl/shader/fs_yuv.frag",GL_FRAGMENT_SHADER);
	glAttachShader(v->shd_yuv,vs);
	glAttachShader(v->shd_yuv,fs);
    glLinkProgram(v->shd_yuv);
    print_link_log("shd_yuv",v->shd_yuv);

	glUseProgram(v->shd_yuv);

	int i;
	i=glGetUniformLocation(v->shd_yuv,"projection");glUniformMatrix4fv(i,1,GL_FALSE,v->projection);
	i=glGetUniformLocation(v->shd_yuv,"tex_y");glUniform1i(i,v->act_y);
	i=glGetUniformLocation(v->shd_yuv,"tex_u");glUniform1i(i,v->act_u);
	i=glGetUniformLocation(v->shd_yuv,"tex_v");glUniform1i(i,v->act_v);
	v->idx_yuv_screen=glGetUniformLocation(v->shd_yuv,"screen");glUniform2f(v->idx_yuv_screen,v->res_x,v->res_y);
	v->idx_yuv_format=glGetUniformLocation(v->shd_yuv,"format");glUniform1f(v->idx_yuv_format,v->yuv_format);
	}
	//-----------------

	//-- v->shd_font --
	{
	v->shd_font=glCreateProgram();
	GLuint vs=compile_shader("ogl/shader/vs_font.vert",GL_VERTEX_SHADER);
	GLuint fs=compile_shader("ogl/shader/fs_font.frag",GL_FRAGMENT_SHADER);
	glAttachShader(v->shd_font,vs);
	glAttachShader(v->shd_font,fs);
    glLinkProgram(v->shd_font);
    print_link_log("shd_font",v->shd_font);

	glUseProgram(v->shd_font);

	int i;
	i=v->idx_font_projection=glGetUniformLocation(v->shd_font,"projection");glUniformMatrix4fv(i,1,GL_FALSE,v->font_projection);
	i=glGetUniformLocation(v->shd_font,"tex_font");glUniform1i(i,v->act_font);
	i=glGetUniformLocation(v->shd_font,"tex_rgba16i");glUniform1i(i,v->act_font_rgba16i);
	i=glGetUniformLocation(v->shd_font,"tex_pal");glUniform1i(i,v->act_font_pal);
	v->idx_font_screen=glGetUniformLocation(v->shd_font,"screen");glUniform2f(v->idx_font_screen,v->screen_x,v->screen_y);
	v->idx_font_char_size=glGetUniformLocation(v->shd_font,"font_char_size");glUniform1f(v->idx_font_char_size,v->font_char_size);
	v->idx_font_alpha_cut=glGetUniformLocation(v->shd_font,"alpha_cut");glUniform1f(v->idx_font_alpha_cut,v->font_alpha_cut);
	}
	//-----------------

	//-- v->shd_rescale --
	{
	v->shd_rescale=glCreateProgram();
	GLuint vs=compile_shader("ogl/shader/vs_rescale.vert",GL_VERTEX_SHADER);
	GLuint fs=compile_shader("ogl/shader/fs_rescale.frag",GL_FRAGMENT_SHADER);
	glAttachShader(v->shd_rescale,vs);
	glAttachShader(v->shd_rescale,fs);
    glLinkProgram(v->shd_rescale);
    print_link_log("shd_rescale",v->shd_rescale);

	glUseProgram(v->shd_rescale);

	int i;
	i=glGetUniformLocation(v->shd_rescale,"projection");glUniformMatrix4fv(i,1,GL_FALSE,v->projection);
	i=glGetUniformLocation(v->shd_rescale,"tex_input");glUniform1i(i,v->act_screen);
	i=glGetUniformLocation(v->shd_rescale,"input_size");glUniform2f(i,v->img_x,v->img_y);
	i=glGetUniformLocation(v->shd_rescale,"fb_size");glUniform2f(i,v->img_x,v->img_y);
	v->idx_rescale_screen=glGetUniformLocation(v->shd_rescale,"screen");glUniform4f(v->idx_rescale_screen,0,0,v->res_x,v->res_y);
	}
	//-----------------

	glUseProgram(0);

	glEnable(GL_SCISSOR_TEST);

	v->screen_saturation=0; //-1,1
	v->screen_contrast=0;   //-1,1
	v->screen_brightness=0;  //-1,1
	v->screen_gamma=1.0;    //0,10

	glViewport(0,0,v->screen_x,v->screen_y);

	glFinish();

	int y_size=v->img_x*v->img_y;
	int u_size=v->img_x*v->img_y;
	int v_size=v->img_x*v->img_y;
	v->y_data=malloc(y_size+u_size+v_size);
	v->u_data=&v->y_data[y_size];
	v->v_data=&v->y_data[y_size+u_size];
	v->y_line_size=v->img_x;
	v->u_line_size=v->img_x;
	v->v_line_size=v->img_x;

	memset(v->y_data,0,y_size+u_size+v_size);

	v->yuv_update=0;

	v->font_data_buf=malloc((256*256*2)*4);

	v->font_start_char=0;
	v->font_end_char=0;
}
