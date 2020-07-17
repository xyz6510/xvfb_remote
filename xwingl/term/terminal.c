
void term_buffer_clear()
{
	TERMINAL *tr=&terminal;

	memset(tr->buf,32,terminal_size*sizeof(uint8_t));
	int i;
	for (i=0;i<terminal_size;i++) {
		tr->cbuf[i]=0x010f;
	}
}

void term_clear()
{
	TERMINAL *tr=&terminal;

	tr->cx=0;
	tr->cy=0;
	tr->bpos=0;
	int len=tr->w*tr->h;
	int i;
	for (i=0;i<len;i++) {
		tr->buf[tr->tbs+i]=32;
		tr->cbuf[tr->tbs+i]=tr->fgbg;
	}
}

void term_reset()
{
	TERMINAL *tr=&terminal;
	VIDEO_VAR *v=&video_var;

	tr->fgbg=0x0e06;
	tr->bpos=0;
	tr->x=0;
	tr->y=0;
	tr->w=40;
	tr->h=25;
	tr->cfgbg=0x1320;//0x2003;
	tr->cidx=127;//175;//145;
	tr->cx=0;
	tr->cy=0;
	tr->tbs=0;
	tr->active=0;

	v->font_char_size=8*3;//24;
	v->font_alpha_cut=0.5;

	term_buffer_clear();
	term_clear();
}

void term_update()
{
	TERMINAL *tr=&terminal;
	VIDEO_VAR *v=&video_var;

	if (tr->active==0) return;

	int i,j,x,y;
	x=tr->x;y=tr->y;
	short int *xyc=v->font_data_buf;
	int fs=v->font_char_size;
	int pos=0;
	for (i=0;i<tr->h;i++) {
		x=tr->x;
		for (j=0;j<tr->w;j++) {
			xyc[pos*4+0]=x;
			xyc[pos*4+1]=y;
			uint8_t chr=tr->buf[tr->tbs+pos];
			if (chr<32) chr=32;
			if (chr>207) chr=207;
			chr=chr-32;
			xyc[pos*4+2]=chr;
			xyc[pos*4+3]=tr->cbuf[tr->tbs+pos];
			x+=fs;
			pos++;
		}
		y+=fs;
	}
	int len=tr->w*tr->h;
	xyc[len*4+0]=tr->x+tr->cx*fs;
	xyc[len*4+1]=tr->y+tr->cy*fs;
	int chr=tr->cidx;
	if (chr<32) chr=32;
	if (chr>207) chr=207;
	chr=chr-32;
	xyc[len*4+2]=chr;
	xyc[len*4+3]=tr->cfgbg;

	len++;
	int tw=len,th=1;
	if (len>256) {
		tw=256;
		th=(len/256)+1;
	}
	update_texture(v->act_font_rgba16i,v->font_tex_rgba16i,0,0,tw,th,0,GL_RGBA_INTEGER,GL_SHORT,v->font_data_buf);
}

void term_print(char *str,int fgbg,int scroll)
{
	TERMINAL *tr=&terminal;
	int len=strlen(str);

	int i;
	for (i=0;i<len;i++) {
		tr->buf[tr->tbs+tr->bpos+i]=str[i];
	}

	uint16_t fgbg1=fgbg;
	if (fgbg==-1) fgbg1=tr->fgbg;

	if (fgbg!=-2) {
		int i;
		for (i=0;i<len;i++) {
			tr->cbuf[tr->tbs+tr->bpos+i]=fgbg1;
		}
	}

	tr->bpos+=len;
	tr->cx+=len;
	int w=tr->w-1;
	int h=tr->h-1;
	if (tr->cx>w) {
		int ya=tr->cx/tr->w;
		int x=tr->cx-(ya*tr->w);

		tr->cy+=ya;
		tr->cx=x;
		if (tr->cy>h) {
			if (scroll) {
				int scr=tr->cy-h;
				tr->cy=h;
				int num_scr=scr*tr->w;
				uint8_t *p1=&tr->buf[tr->tbs+num_scr];
				uint8_t *p0=&tr->buf[tr->tbs];
				uint16_t *c1=&tr->cbuf[tr->tbs+num_scr];
				uint16_t *c0=&tr->cbuf[tr->tbs];
				int i;
				for (i=0;i<tr->w*tr->h;i++) {
					p0[i]=p1[i];
					c0[i]=c1[i];
				}
				tr->bpos=tr->bpos-num_scr;
				for (i=0;i<num_scr;i++) {
					tr->buf[tr->tbs+tr->bpos+i]=32;
					tr->cbuf[tr->tbs+tr->bpos+i]=tr->fgbg;
				}
			} else {
				tr->cx=tr->w-1;
				tr->cy=tr->h-1;
				tr->bpos=tr->cy*tr->w+tr->cx;
			}
		}
	}
}

void term_print_at(int x,int y,char *str,int fgbg,int scroll)
{
	TERMINAL *tr=&terminal;

	int ox=tr->cx;
	int oy=tr->cy;
	int obpos=tr->bpos;

	if (x==-1) {
		x=(tr->w-strlen(str))/2;
		if (x<0) x=0;
	}
	if (y==-1) {
		y=(tr->h-(strlen(str)/tr->w))/2;
		if (y<0) y=0;
	}
	if (x!=-2) tr->cx=x;
	if (y!=-2) tr->cy=y;
	if ((x!=-2)||(y!=-2)) tr->bpos=(y*tr->w)+x;

	term_print(str,fgbg,scroll);

	tr->cx=ox;
	tr->cy=oy;
	tr->bpos=obpos;
}

void term_show(int active)
{
	TERMINAL *tr=&terminal;
	VIDEO_VAR *v=&video_var;

	if (active==-1) {
		tr->active^=1;
		active=tr->active;
	}
	if (active) {
		v->font_start_char=0;
		v->font_end_char=(tr->w*tr->h)+1;
	} else {
		v->font_start_char=0;
		v->font_end_char=0;
	}
	tr->active=active;
	term_update();
}
