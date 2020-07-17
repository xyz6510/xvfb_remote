
void set_fullscreen(int f,int resizable)
{
	Display *d=screen.d;
	Window w=screen.w;

	if (resizable==0) {
		XSizeHints *sizehints=XAllocSizeHints();
		memset(sizehints,0,sizeof(XSizeHints));
		long userhints;
		XGetWMNormalHints(d,w,sizehints,&userhints);
		if (f) {
			sizehints->flags|=PMinSize|PMaxSize;
			sizehints->min_width=100;
			sizehints->max_width=32768;
			sizehints->min_height=100;
			sizehints->max_height=32768;
		} else {
			sizehints->flags|=PMinSize|PMaxSize;
			sizehints->min_width=sizehints->max_width=screen.sww;
			sizehints->min_height=sizehints->max_height=screen.swh;
		}
		XSetWMNormalHints(d,w,sizehints);
		XFree(sizehints);
	}

	Atom wm_state=XInternAtom(d,"_NET_WM_STATE", False);
	Atom fs=XInternAtom(d,"_NET_WM_STATE_FULLSCREEN",False);
	XEvent e;
	memset(&e,0,sizeof(e));
	e.type=ClientMessage;
	e.xclient.window=w;
	e.xclient.message_type=wm_state;
	e.xclient.format=32;
	e.xclient.data.l[0]=f;
	e.xclient.data.l[1]=fs;
	e.xclient.data.l[2]=0;
	XSendEvent(d,DefaultRootWindow(d),False,SubstructureRedirectMask|SubstructureNotifyMask,&e);

	if ((f==0)&&(resizable==0)) XResizeWindow(d,w,screen.sww,screen.swh);
}
