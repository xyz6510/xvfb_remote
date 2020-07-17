
void draw_yuv()
{
	VIDEO_VAR *v=&video_var;

	static int yuv_update=0;
	if (v->yuv_update!=yuv_update) {
		int yy=v->res_y;
		int uvy=v->res_y/v->yuv_format;

		update_texture(v->act_y,v->y_tex,0,0,v->y_line_size,yy,  0,GL_RED,GL_UNSIGNED_BYTE,v->y_data);
		update_texture(v->act_u,v->u_tex,0,0,v->u_line_size,uvy,0,GL_RED,GL_UNSIGNED_BYTE,v->u_data);
		update_texture(v->act_v,v->v_tex,0,0,v->v_line_size,uvy,0,GL_RED,GL_UNSIGNED_BYTE,v->v_data);

		yuv_update=v->yuv_update;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,v->fb_screen);
//	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,v->res_x,v->res_y);
	glScissor(0,0,v->res_x,v->res_y);
//	glClearColor(0,1,0,1);
//	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(v->shd_yuv);
	glBindVertexArray(v->va0);
	glDrawArrays(GL_TRIANGLES,0,6);
}
