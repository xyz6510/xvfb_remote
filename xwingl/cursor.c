
#include <X11/Xcursor/Xcursor.h>

void cursor(SRV *srv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	Display *disp=XOpenDisplay(NULL);
	Window win=screen.w;

	xwg->conn_cursor=connect_to(srv->str,xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num+srv->p);

	int *xcbuf=malloc(64*1024);
	int e=0;
	while (xwg->end==0) {
		int type;
		e=read_data(xwg->conn_cursor,&type,4,NULL);if (e<=0) break;
		if (type==88) {
			int w,h,xh,yh;
			e=read_data(xwg->conn_cursor,&w,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_cursor,&h,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_cursor,&xh,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_cursor,&yh,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_cursor,xcbuf,w*h*4,NULL);if (e<=0) break;

			XcursorImage *img;
			int scale=2;
			if (xwg->cursor_scale) scale=xwg->cursor_scale;
			int nw=w*scale;
			int nh=h*scale;
			img=XcursorImageCreate(nw,nh);
			img->xhot=xh*scale;
			img->yhot=yh*scale;
			if (scale>1) {
				int i,j;
				int *out=(int*)img->pixels;
				int *in=xcbuf;
				for (i=0;i<nh;i+=scale) {
					int *out_o=out;
					for (j=0;j<w;j++) {
						int k;
						for (k=0;k<scale;k++) {
							*out=in[j];
							out++;
						}
					}
					for (j=0;j<scale-1;j++) {
						memcpy(out,out_o,nw*4);
						out_o=out;
						out+=nw;
					}
					in+=w;
				}
			} else {
				memcpy(img->pixels,xcbuf,w*h*4);
			}
			Cursor cr;
			cr=XcursorImageLoadCursor(disp,img);
			XcursorImageDestroy(img);
			if (xwg->terminal_inside==0) XDefineCursor(disp,win,cr);
			if (xwg->win_cursor) XFreeCursor(disp,xwg->win_cursor);
	    		xwg->win_cursor=cr;
    			XSync(disp,False);
		}
	}
	XCloseDisplay(disp);
	free(xcbuf);
	close_socket(xwg->conn_cursor);
	xwg->end=1;
	screen.end=1;

	printf("connection cursor close\n");
}
