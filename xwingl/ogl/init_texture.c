
void init_texture(int active,int w,int h,GLuint tex,GLuint internal,GLuint format,GLuint type,int zero,int linear_min,int linear_max)
{
	glActiveTexture(GL_TEXTURE0+active);
	glBindTexture(GL_TEXTURE_2D,tex);

	if (linear_min) glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	else 			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	if (linear_max)	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	else 			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,internal,w,h,0,format,type,NULL);
	if (zero) {
		int bpp;
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_RED_SIZE,&bpp);
		char *data=malloc(w*4*bpp);
		memset(data,0,w*4*bpp);
		int i;
		for (i=0;i<h;i++) {
			glTexSubImage2D(GL_TEXTURE_2D,0,0,i,w,1,format,type,data);
		}
		free(data);
	}
}
