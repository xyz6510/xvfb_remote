
void draw_fb_screen()
{
	VIDEO_VAR *v=&video_var;

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,v->screen_x,v->screen_y);
	glScissor(0,0,v->screen_x,v->screen_y);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(v->shd_screen);
	glUniform1f(v->idx_saturation,v->screen_saturation);
	glUniform1f(v->idx_contrast,v->screen_contrast);
	glUniform1f(v->idx_brightness,v->screen_brightness);
	glUniform1f(v->idx_gamma,v->screen_gamma);
	glBindVertexArray(v->va0);
	glDrawArrays(GL_TRIANGLES,0,6);
}
