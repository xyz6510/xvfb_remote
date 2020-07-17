
#include <X11/extensions/Xfixes.h>

void clipboard_change()
{
	SCR *sc=&screen;

	Display *d=XOpenDisplay(NULL);
	Window w=DefaultRootWindow(d);

	Atom XA_UTF8=XInternAtom(d,"UTF8_STRING",True);
	Atom CLIP_CHANGE=XInternAtom(d,"CLIP_CHANGE",False);

	XFixesSelectSelectionInput(d,w,XA_PRIMARY,XFixesSetSelectionOwnerNotifyMask);
	while (sc->end==0) {
		XEvent ev;
        XNextEvent(d,&ev);
		if (((XFixesSelectionNotifyEvent*)&ev)->selection==XA_PRIMARY) {
			XConvertSelection(d,XA_PRIMARY,XA_UTF8,CLIP_CHANGE,sc->win,CurrentTime);
		}
	}
}
