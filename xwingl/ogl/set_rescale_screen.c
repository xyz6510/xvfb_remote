
void set_rescale_screen(int x,int y,int w,int h)
{
	VIDEO_VAR *v=&video_var;

	glUseProgram(v->shd_rescale);

	glUniform4f(v->idx_rescale_screen,x,y,w,h);

	glUseProgram(0);
}
