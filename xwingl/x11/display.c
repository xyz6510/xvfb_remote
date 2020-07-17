
int xerr(Display *d,XErrorEvent *e)
{
	return 0;
}

void open_display(char *name,int resizable,int borderless)
{
	memset(&screen,0,sizeof(SCR));
	screen.keybuf=malloc(keybuf_size);
	screen.mousebuf=malloc(mousebuf_size);
	memset(screen.keybuf,0,keybuf_size);
	memset(screen.mousebuf,0,mousebuf_size);

	XInitThreads();
	XSetErrorHandler(xerr);
	screen.d=XOpenDisplay(NULL);

	screen.ww=640;
	screen.wh=360;
	screen.sww=screen.ww;
	screen.swh=screen.wh;

	screen.root=DefaultRootWindow(screen.d);
	screen.w=XCreateSimpleWindow(screen.d,screen.root,0,0,screen.ww,screen.wh,0,0,0);
	Display *d=screen.d;
	Window w=screen.w;


	screen.a=XInternAtom(d,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(d,w,&screen.a,1);

	XMapWindow(d,w);
	XStoreName(d,w,name);

	if (resizable==0) {
		XSizeHints *sizehints=XAllocSizeHints();
		long userhints;
		XGetWMNormalHints(d,w,sizehints,&userhints);
		sizehints->min_width=sizehints->max_width=screen.ww;
		sizehints->min_height=sizehints->max_height=screen.wh;
		sizehints->flags|=PMinSize|PMaxSize;
		XSetWMNormalHints(d,w,sizehints);
		XFree(sizehints);
	}
	if (borderless) {
		struct MwmHints {
	    	unsigned long flags;
	    	unsigned long functions;
		    unsigned long decorations;
    		long input_mode;
		    unsigned long status;
		};
		enum {
		    MWM_HINTS_FUNCTIONS=(1L<<0),
		    MWM_HINTS_DECORATIONS=(1L<<1),
		    MWM_FUNC_ALL=(1L<<0),
		    MWM_FUNC_RESIZE=(1L<<1),
		    MWM_FUNC_MOVE=(1L<<2),
		    MWM_FUNC_MINIMIZE=(1L<<3),
		    MWM_FUNC_MAXIMIZE=(1L<<4),
		    MWM_FUNC_CLOSE=(1L<<5)
		};

		Atom mwmHintsProperty=XInternAtom(d,"_MOTIF_WM_HINTS",0);
		struct MwmHints hints;
		hints.flags=MWM_HINTS_DECORATIONS;
		hints.decorations=0;
		XChangeProperty(d,w,mwmHintsProperty,mwmHintsProperty,32,
			PropModeReplace,(unsigned char*)&hints,5);
	}

	pthread_create(&screen.input_t,NULL,(void*)&get_input,NULL);

	usleep(10000);

	XSync(d,False);

	GLint att[]={GLX_RGBA,GLX_DEPTH_SIZE,0,GLX_DOUBLEBUFFER,None};
	XVisualInfo *vi=glXChooseVisual(d,0,att);
	screen.g=glXCreateContext(d,vi,NULL,GL_TRUE);
	glXMakeCurrent(d,w,screen.g);

}

void check_wm_delete_window()
{
	Display *d=screen.d;
	Window w=screen.w;

	XEvent ev;
   	if (XCheckTypedWindowEvent(d,w,ClientMessage,&ev)) {
		if ((Atom)ev.xclient.data.l[0]==screen.a) {
			screen.end=1;
		}
	}
}

void close_display()
{
	Display *d=screen.d;
	Window w=screen.w;
	GLXContext g=screen.g;

	screen.end=1;
	printf(":wait input exit\n");
	XEvent ev;
	ev.type=DestroyNotify;
	ev.xdestroywindow.window=w;
	XSendEvent(d,w,False,StructureNotifyMask,&ev);
	XFlush(d);
	pthread_join(screen.input_t,NULL);

	glXMakeCurrent(d,None,NULL);
	glXDestroyContext(d,g);
	XUnmapWindow(d,w);
	XDestroyWindow(d,w);
	XCloseDisplay(d);
}
