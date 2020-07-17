
void update_texture(int active,GLuint tex,int x,int y,int w,int h,int row,GLenum format,GLenum type,void *data)
{
	glActiveTexture(GL_TEXTURE0+active);
	glBindTexture(GL_TEXTURE_2D,tex);
	glPixelStorei(GL_UNPACK_ROW_LENGTH,row);
	glTexSubImage2D(GL_TEXTURE_2D,0,x,y,w,h,format,type,data);
	glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
}
