
void check_wm_selection_notify()
{
	SCR *sc=&screen;

	Display *d=sc->d;
	Window w=sc->win;

	Atom target;
	int format;
	unsigned long size,after;
	unsigned char *data;

	XEvent ev;
   	if (XCheckTypedWindowEvent(d,w,SelectionNotify,&ev)) {
   		if (ev.xselection.property) {
			XGetWindowProperty(ev.xselection.display,ev.xselection.requestor,
				ev.xselection.property,0,4096,False,AnyPropertyType,&target,
				&format,&size,&after,&data);
			char *name=XGetAtomName(d,ev.xselection.property);
			//printf("data:[%s]=[%s]%li\n",name,data,size);
			if (size>0) {
				memcpy(sc->clip_data,data,size);
				sc->clip_data[size]=0;
				sc->clip_size=size;
				sc->send_clipboard_data=1;
			}
			XFree(data);
			XFree(name);
			XDeleteProperty(ev.xselection.display,ev.xselection.requestor,ev.xselection.property);
   		}
   	}
}


int check_wm_selection_request()
{
	SCR *sc=&screen;

	Display *d=sc->d;
	Window w=sc->win;

	int ret=0;
	XEvent ev;
	if (XCheckTypedWindowEvent(d,w,SelectionRequest,&ev)) {
		XSelectionRequestEvent *xsr=&ev.xselectionrequest;
		XSelectionEvent evs;
		memset(&evs,0,sizeof(evs));

		evs.type=SelectionNotify;
		evs.display=xsr->display;
		evs.requestor=xsr->requestor;
		evs.selection=xsr->selection;
		evs.time=xsr->time;
		evs.target=xsr->target;
		evs.property=xsr->property;

		if (w!=evs.requestor) {
			if (evs.target==sc->TARGETS) {
				Atom sup[]={sc->UTF8,sc->UTF8_1};
				XChangeProperty(evs.display,evs.requestor,evs.property,evs.target,32,
					PropModeReplace,(unsigned char*)&sup,2);
					//printf("send targets ");
			} else if ((evs.target==sc->UTF8)||(evs.target==sc->UTF8_1)) {
				XChangeProperty(evs.display,evs.requestor,evs.property,sc->UTF8,8,
					PropModeReplace,sc->clip_data,sc->clip_size);
					//printf("send utf8 ");
			} else {
				evs.property=None;
				//printf("send none ");
			}
			ret=1;
			XSendEvent(d,evs.requestor,False,NoEventMask,(XEvent*)&evs);
			XSync(d,False);
		}
		//printf("\n");
	}
	return ret;
}

void typed_events()
{
	SCR *sc=&screen;

    while (sc->end==0) {
    	check_wm_selection_notify();
    	if (check_wm_selection_request()) usleep(1000);
		else usleep(10000);
    }
}
