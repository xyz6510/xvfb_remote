
void set_window_size(unsigned int ww,unsigned int wh,int resizable)
{
	Display *d=screen.d;
	Window w=screen.w;

	screen.ww=ww;
	screen.wh=wh;
	screen.sww=ww;
	screen.swh=wh;

	if (resizable==0) {
		XSizeHints *sizehints=XAllocSizeHints();
		long userhints;
		XGetWMNormalHints(d,w,sizehints,&userhints);
		sizehints->min_width=sizehints->max_width=ww;
		sizehints->min_height=sizehints->max_height=wh;
		sizehints->flags|=PMinSize|PMaxSize;
		XSetWMNormalHints(d,w,sizehints);
		XFree(sizehints);
	}

	XResizeWindow(screen.d,screen.w,ww,wh);
}
