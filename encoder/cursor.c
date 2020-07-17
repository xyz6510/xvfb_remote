
void cursor(SRV *srv)
{
	SCR *sc=&screen;

	Display *disp=XOpenDisplay(NULL);

	int *xcbuf=malloc(64*1024);

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num+srv->p);
	int client_socket;
l1:
	client_socket=accept_connection(sck,srv->str,sc->ip_addr_str,sc->ip_port_num+srv->p);

	int cursor_serial=0;
	int e=0;
	int live_cnt=0;
	while (sc->end==0) {
		XFixesCursorImage *xc=XFixesGetCursorImage(disp);
		if (xc->cursor_serial!=cursor_serial) {
			cursor_serial=xc->cursor_serial;
			int len=xc->width*xc->height;
			if (sizeof(long)==8) {
				int *p=(int*)xc->pixels;
				int i;
				for (i=0;i<len;i++) {
					xcbuf[i]=p[i*2];
				}
			} else {
				memcpy(xcbuf,xc->pixels,len*4);
			}
			int w=xc->width;
			int h=xc->height;
			int xh=xc->xhot;
			int yh=xc->yhot;
		    free(xc);xc=NULL;

			int type=88;
			e=write_data(client_socket,&type,4,NULL);if (e<=0) break;
			e=write_data(client_socket,&w,4,NULL);if (e<=0) break;
			e=write_data(client_socket,&h,4,NULL);if (e<=0) break;
			e=write_data(client_socket,&xh,4,NULL);if (e<=0) break;
			e=write_data(client_socket,&yh,4,NULL);if (e<=0) break;
			e=write_data(client_socket,xcbuf,len*4,NULL);if (e<=0) break;
		} else {
			live_cnt++;
			if (live_cnt>3) {
				live_cnt=0;
				int type=44;
				e=write_data(client_socket,&type,4,NULL);if (e<=0) break;
			}
		}
		if (xc) free(xc);
	    usleep(40000);
	}
	printf("connection cursor closed\n");
	close_socket(client_socket);
	if (sc->end==0) goto l1;
	printf("cursor exit\n");
	free(xcbuf);
}
