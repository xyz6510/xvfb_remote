
void draw_rescale()
{
	VIDEO_VAR *v=&video_var;

	if ((v->screen_x>v->res_x)||(v->screen_y>v->res_y)||(v->zoom==1)) {
		glBindFramebuffer(GL_FRAMEBUFFER,v->fb_rescale);
//		glBindFramebuffer(GL_FRAMEBUFFER,0);
		int fbx=v->screen_x;
		int fby=v->screen_y;
		if (fbx>v->desktop_x) fbx=v->desktop_x;
		if (fby>v->desktop_y) fby=v->desktop_y;
		glViewport(0,0,fbx,fby);
		glScissor(0,0,fbx,fby);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(v->shd_rescale);
		glBindVertexArray(v->va0);
		glDrawArrays(GL_TRIANGLES,0,6);
	}
}
