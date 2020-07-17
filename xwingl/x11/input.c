
int get_input()
{
	Display *d=XOpenDisplay(NULL);
	Window w=screen.w;

	XSelectInput(d,w,ExposureMask|KeyPressMask|
					 KeyReleaseMask|ButtonPressMask|
					 ButtonReleaseMask|PointerMotionMask|
					 FocusChangeMask|VisibilityChangeMask|
					 StructureNotifyMask|
					 SubstructureNotifyMask|
					 EnterWindowMask|LeaveWindowMask);

	int *keybuf=screen.keybuf;
	int *mousebuf=screen.mousebuf;

	XEvent ev;
	while(screen.end==0) {
		XNextEvent(d,&ev);
		int type=ev.type;
		screen.ev.e++;
		//printf("[%i] ev type:%i ",screen.ev.e,type);
		if ((type==KeyPress)||(type==KeyRelease)) {
			screen.ev.k++;
			//printf(":k[%i] ",screen.ev.k);
			int key=ev.xkey.keycode;
			//printf(":key:%i hex:%x ",key,key);
			//printf(":key:%i hex:%x \n",key,key);
			if (type==KeyPress) {
				if (keybuf[key]<0) keybuf[key]=-keybuf[key];
				keybuf[key]++;
				//printf("press %i ",keybuf[key]);
			}
			if (type==KeyRelease) {
				if (keybuf[key]>0) {
					keybuf[key]=-keybuf[key];
					//printf("release %i ",keybuf[key]);
				}
			}
		}
		if ((type==ButtonPress)||(type==ButtonRelease)||(type==MotionNotify)) {
			screen.ev.m++;
			//printf(":m[%i] mm[%i] ",screen.ev.m,screen.ev.mm);
			if (type==ButtonPress) {
				//printf("mp:%i ",ev.xbutton.button);
				if (ev.xbutton.button==1) mousebuf[0]=1;
				if (ev.xbutton.button==3) mousebuf[1]=1;
				if (ev.xbutton.button==2) mousebuf[2]=1;
				if (ev.xbutton.button==4) mousebuf[3]++;//set to 0 after read;
				if (ev.xbutton.button==5) mousebuf[4]++;//set to 0 after read;
			}
			if (type==ButtonRelease) {
				//printf("mr:%i ",ev.xbutton.button);
				if (ev.xbutton.button==1) mousebuf[0]=-mousebuf[0];
				if (ev.xbutton.button==3) mousebuf[1]=-mousebuf[1];
				if (ev.xbutton.button==2) mousebuf[2]=-mousebuf[2];
			}
			if (type==MotionNotify) {
				screen.ev.mm++;
				//printf("mxy:%i %i ",ev.xmotion.x,ev.xmotion.y);
				mousebuf[10]=ev.xmotion.x;
				mousebuf[11]=ev.xmotion.y;
			}
		}
		if (type==ConfigureNotify) {
			//printf(":configure notify ");
			if ((ev.xconfigure.x!=screen.wx)||(ev.xconfigure.y!=screen.wy)) {
				screen.wx=ev.xconfigure.x;
				screen.wy=ev.xconfigure.y;
				screen.ev.move++;
				//printf("move[%i]:%i %i ",screen.ev.move,screen.wx,screen.wy);
			}
			if ((ev.xconfigure.width!=screen.ww)||(ev.xconfigure.height!=screen.wh)) {
				screen.ww=ev.xconfigure.width;
				screen.wh=ev.xconfigure.height;
				screen.ev.res++;
				//printf("resize[%i]:%i %i ",screen.ev.res,screen.ww,screen.wh);
			}
		}
		if (type==MapNotify) {//on show
			screen.ev.map=1;
			screen.ev.min=0;
			//printf(":map notify map:%i min:%i ",screen.ev.map,screen.ev.min);
		}
		if (type==UnmapNotify) { //on minimize
			screen.ev.map=0;
			screen.ev.min=1;
			//printf(":unmap notify map:%i min:%i ",screen.ev.map,screen.ev.min);
		}
		if (type==EnterNotify) {//mouse enter
			screen.ev.mle=1;
			//printf(":enter notify %i ",screen.ev.mle);
		}
		if (type==LeaveNotify) { //mouse leave
			screen.ev.mle=0;
			//printf(":leave notify %i ",screen.ev.mle);
		}
		if (type==FocusIn) {//
			screen.ev.focus=1;
			//printf(":focus in %i ",screen.ev.focus);
		}
		if (type==FocusOut) { //
			screen.ev.focus=0;
			//printf(":focus out %i ",screen.ev.focus);
		}
		if (type==Expose) {
			screen.ev.expose++;
			//printf(":expose %i ",screen.ev.expose);
		}
		if (type==DestroyNotify) {
			//printf(":win close");
			screen.end=1;
		}
		//printf("\n");
	}
	printf(":input exit\n");
	XCloseDisplay(d);

	return 0;
}
