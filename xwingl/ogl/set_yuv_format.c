
void set_yuv_format(int format)
{
	VIDEO_VAR *v=&video_var;

	v->yuv_format=format;

	glUseProgram(v->shd_yuv);

	glUniform1f(v->idx_yuv_format,v->yuv_format);
	glUniform2f(v->idx_yuv_screen,v->res_x,v->res_y);

	glUseProgram(0);
}
