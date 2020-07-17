
void get_clipboard(SRV *srv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	xwg->conn_get_clipboard=connect_to(srv->str,xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num+srv->p);

	int e=0;
	while (xwg->end==0) {
		int type;
		e=read_data(xwg->conn_get_clipboard,&type,4,NULL);if (e<=0) break;
		if (type==92) {
			int size;
			xwg->clip_size=0;
			e=read_data(xwg->conn_get_clipboard,&size,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_get_clipboard,xwg->clip_data,size,NULL);if (e<=0) break;
			xwg->clip_data[size]=0;
			xwg->clip_size=size;
			//printf("get clipboard:[%s]\n",clip_data);
			XSetSelectionOwner(screen.d,XA_PRIMARY,screen.w,CurrentTime);
			if (XGetSelectionOwner(screen.d,XA_PRIMARY)!=screen.w) {
				printf("set selection owner fail\n");
			}
		}
	}
	close_socket(xwg->conn_get_clipboard);
	printf("connection conn_get_clipboard close\n");
	xwg->end=1;
	screen.end=1;
}
