
void get_window_pos(int *x,int *y)
{
	Window w;
	unsigned int wx,wy;
	unsigned int bw,bpp;

	XGetGeometry(screen.d,screen.w,&w,x,y,&wx,&wy,&bw,&bpp);
}
