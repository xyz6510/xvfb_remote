
void print_gl_info()
{
	int gl_get;
	int gl_get2[2];

	printf("GL_VENDOR:\t\t\t\t%s\n",glGetString(GL_VENDOR));
	printf("GL_RENDERER:\t\t\t\t%s\n",glGetString(GL_RENDERER));
	printf("GL_VERSION:\t\t\t\t%s\n",glGetString(GL_VERSION));
	printf("GL_SHADING_LANGUAGE_VERSION:\t\t%s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&gl_get);
	printf("GL_MAX_TEXTURE_IMAGE_UNITS:\t\t%i\n",gl_get);

//	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&gl_get);
//	printf("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:\t%i\n",gl_get);

	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,&gl_get);
	printf("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:\t%i\n",gl_get);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&gl_get);
	printf("GL_MAX_TEXTURE_SIZE:\t\t\t%ix%i\n",gl_get,gl_get);

	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,&gl_get);
	printf("GL_MAX_ARRAY_TEXTURE_LAYERS:\t\t%i\n",gl_get);

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&gl_get);
	printf("GL_MAX_RENDERBUFFER_SIZE:\t\t%ix%i\n",gl_get,gl_get);

	glGetIntegerv(GL_MAX_VIEWPORT_DIMS,gl_get2);
	printf("GL_MAX_VIEWPORT_DIMS:\t\t\t%ix%i\n",gl_get2[0],gl_get2[1]);
}
