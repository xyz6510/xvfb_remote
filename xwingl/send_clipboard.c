
void send_clipboard(SRV *srv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	xwg->conn_send_clipboard=connect_to(srv->str,xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num+srv->p);

	int e=0;
	while (xwg->end==0) {
		if (xwg->send_clipboard_data==1) {
			xwg->send_clipboard_data=0;
			e=write_data(xwg->conn_send_clipboard,&xwg->clip_size,4,NULL);if (e<=0) break;
			e=write_data(xwg->conn_send_clipboard,xwg->clip_data,xwg->clip_size,NULL);if (e<=0) break;
		}
		usleep(40000);
	}
	close_socket(xwg->conn_send_clipboard);
	xwg->end=1;
	screen.end=1;

	printf("connection conn_send_clipboard close\n");
}
