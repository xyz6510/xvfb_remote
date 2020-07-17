
GLuint compile_shader(char *file,GLuint type)
{
	int max_size=1024*1024;
	char *shd_text=malloc(max_size);
	memset(shd_text,0,max_size);
	int f=open(file,O_RDONLY);
	if (f<0) {
		printf("error open %s\n",file);
		exit(-1);
	}
	read(f,shd_text,max_size);
	close(f);

	GLuint shader;
	const char *shader_array[1]={shd_text};
	shader=glCreateShader(type);
	glShaderSource(shader,1,shader_array,NULL);
	glCompileShader(shader);
	free(shd_text);

	int i;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&i);
	char *s=malloc(32768);
	memset(s,0,32768);
	glGetShaderInfoLog(shader,32768,NULL,s);
	if (strlen(s)>0) {
		printf("Compile Log \'%s\': %s\n",file,s);
		exit(-1);
	}
	free(s);

	return shader;
}

void print_link_log(char *name,GLuint program_object)
{
	char *s=malloc(32768);
	memset(s,0,32768);
	glGetProgramInfoLog(program_object,32768,NULL,s);
	if (strlen(s)>0) {
		printf("Link Log: \'%s\' %s\n",name,s);
		exit(-1);
	}
	free(s);
}
