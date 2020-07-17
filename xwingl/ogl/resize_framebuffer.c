
void resize_framebuffer(int screen_x,int screen_y)
{
	VIDEO_VAR *v=&video_var;

	v->screen_x=screen_x;
	v->screen_y=screen_y;

	ortho2d(0,0,v->screen_x,v->screen_y,v->font_projection,1);

	glUseProgram(v->shd_font);

	glUniformMatrix4fv(v->idx_font_projection,1,GL_FALSE,v->font_projection);

	glUniform2f(v->idx_font_screen,v->screen_x,v->screen_y);

	glUseProgram(v->shd_screen);

	if ((v->screen_x>v->res_x)||(v->screen_y>v->res_y)||(v->zoom==1)) {
		glUniform2f(v->idx_screen,v->screen_x,v->screen_y);
		glUniform1i(v->idx_tex_screen,v->act_rescale);
		glUniform1f(v->idx_linear,0);
	} else {
		glUniform2f(v->idx_screen,v->res_x,v->res_y);
		glUniform1i(v->idx_tex_screen,v->act_screen);
		glUniform1f(v->idx_linear,1);
	}

	glUseProgram(0);

}
