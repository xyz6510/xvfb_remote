
void draw_font()
{
	VIDEO_VAR *v=&video_var;

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,v->screen_x,v->screen_y);
	glScissor(0,0,v->screen_x,v->screen_y);
	glUseProgram(v->shd_font);
	glUniform1f(v->idx_font_char_size,v->font_char_size);
	glUniform1f(v->idx_font_alpha_cut,v->font_alpha_cut);
	glBindVertexArray(v->va1);
	int num=v->font_end_char - v->font_start_char;
	glDrawArrays(GL_TRIANGLES,v->font_start_char*6,num*6);
}
