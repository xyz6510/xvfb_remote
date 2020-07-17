
void glerr(int n)
{
	int err=glGetError();
	if (err!=0) {
		printf("n:%i err:%i\n",n,err);
		exit(-1);
	}
}
