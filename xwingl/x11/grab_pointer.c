
void grab_pointer(int c)
{
	Display *d=screen.d;
	Window w=screen.w;

	if (c) {
		XGrabPointer(d,w,True,
			ButtonPressMask|ButtonReleaseMask|PointerMotionMask|FocusChangeMask,
			GrabModeAsync,GrabModeAsync,w,None,CurrentTime);
	} else {
		XUngrabPointer(d,CurrentTime);
	}

}
