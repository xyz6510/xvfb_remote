
void check_wm_selection_notify()
{
	XWINGL_VAR *xwg=&xwingl_var;

	Display *d=screen.d;
	Window w=screen.w;

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
				memcpy(xwg->clip_data,data,size);
				xwg->clip_data[size]=0;
				xwg->clip_size=size;
				xwg->send_clipboard_data=1;
			}
			XFree(data);
			XFree(name);
			XDeleteProperty(ev.xselection.display,ev.xselection.requestor,ev.xselection.property);
   		}
   	}
}

int check_wm_selection_request()
{
	XWINGL_VAR *xwg=&xwingl_var;

	Display *d=screen.d;
	Window w=screen.w;

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
			if (evs.target==xwg->TARGETS) {
				Atom sup[]={xwg->UTF8,xwg->UTF8_1};
				XChangeProperty(evs.display,evs.requestor,evs.property,evs.target,32,
					PropModeReplace,(unsigned char*)&sup,2);
					//printf("send targets ");
			} else if ((evs.target==xwg->UTF8)||(evs.target==xwg->UTF8_1)) {
				XChangeProperty(evs.display,evs.requestor,evs.property,xwg->UTF8,8,
					PropModeReplace,xwg->clip_data,xwg->clip_size);
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
	XWINGL_VAR *xwg=&xwingl_var;

    while (xwg->end==0) {
    	check_wm_delete_window();
    	check_wm_selection_notify();
    	if (check_wm_selection_request()) usleep(1000);
		else usleep(10000);
    }
}
