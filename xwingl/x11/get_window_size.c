
void get_window_size(int *x,int *y)
{
	Window w;
	int px,py;
	unsigned int bw,bpp;

	XGetGeometry(screen.d,screen.w,&w,&px,&py,(unsigned int*)x,(unsigned int*)y,&bw,&bpp);
}
