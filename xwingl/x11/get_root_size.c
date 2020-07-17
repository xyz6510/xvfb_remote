
void get_root_size(int *x,int *y)
{
	Window w;
	int px,py;
	unsigned int bw,bpp;

	XGetGeometry(screen.d,screen.root,&w,&px,&py,(unsigned int*)x,(unsigned int*)y,&bw,&bpp);
}
