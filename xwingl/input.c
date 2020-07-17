
void input(SRV *srv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	SCR *sc=&screen;

	xwg->conn_input=connect_to(srv->str,xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num+srv->p);

	void release_key(int k)
	{
		int type=3;
		int key=k;
		int state=-1;
		write_data(xwg->conn_input,&type,4,NULL);
		write_data(xwg->conn_input,&key,4,NULL);
		write_data(xwg->conn_input,&state,4,NULL);
	}
	void release_mouse(int b)
	{
		int type=2;
		int button=b;
		int state=0;
		write_data(xwg->conn_input,&type,4,NULL);
		write_data(xwg->conn_input,&button,4,NULL);
		write_data(xwg->conn_input,&state,4,NULL);
	}
	int i;
	for (i=0;i<256;i++) {
		release_key(i);
	}
	int kbuf[256];
	int mbuf[256];
	for (i=0;i<256;i++) {
		kbuf[i]=0;
		mbuf[i]=0;
	}
	int *keybuf=screen.keybuf;
	int *mousebuf=screen.mousebuf;
	int focus=0;
	int e;
	while (xwg->end==0) {
		int type;
		if ((mbuf[10]!=mousebuf[10])||(mbuf[11]!=mousebuf[11])) {
			type=1;
			mbuf[10]=mousebuf[10];
			mbuf[11]=mousebuf[11];
			int x=mbuf[10];
			int y=mbuf[11];
			int xp=x*((float)xwg->srv_w/(float)sc->ww);
			int yp=y*((float)xwg->srv_h/(float)sc->wh);
			if (xwg->zoom_active==1) {
				xp=xwg->zoom_x+x*((float)xwg->zoom_w/(float)sc->ww);
				yp=xwg->zoom_y+y*((float)xwg->zoom_h/(float)sc->wh);
			}
			if (xwg->terminal_inside==0) {
				e=write_data(xwg->conn_input,&type,4,NULL);if (e<=0) break;
				e=write_data(xwg->conn_input,&xp,4,NULL);if (e<=0) break;
				e=write_data(xwg->conn_input,&yp,4,NULL);if (e<=0) break;
			}
		}
		for (i=0;i<5;i++) {
			if (mbuf[i]!=mousebuf[i]) {
				mbuf[i]=mousebuf[i];
				type=2;
				int button=i+1;
				if (i==1) button=3;
				if (i==2) button=2;
				int state=0;
				if (mbuf[i]>0) state=1;
				if (xwg->terminal_inside==0) {
					e=write_data(xwg->conn_input,&type,4,NULL);if (e<=0) break;
					e=write_data(xwg->conn_input,&button,4,NULL);if (e<=0) break;
					e=write_data(xwg->conn_input,&state,4,NULL);if (e<=0) break;
				}
			}
		}
		if (xwg->release_mouse_buttons==1) {
			for (i=0;i<3;i++) release_mouse(i);
			xwg->release_mouse_buttons=0;
		}
		if (focus!=xwg->command_active_encoder) {
			focus=xwg->command_active_encoder;
			if ((focus==2)||(focus==-2)) {
				for (i=0;i<256;i++) release_key(i);
			}
		}
		for (i=0;i<256;i++) {
			int type=3;
			int key=i;
			int state=keybuf[i];
			if (kbuf[i]!=keybuf[i]) {
				kbuf[i]=keybuf[i];
				e=write_data(xwg->conn_input,&type,4,NULL);if (e<=0) break;
				e=write_data(xwg->conn_input,&key,4,NULL);if (e<=0) break;
				e=write_data(xwg->conn_input,&state,4,NULL);if (e<=0) break;
				//printf("send:%i %i\n",key,state);
			}
		}
		usleep(10000);
	}
	close_socket(xwg->conn_input);
	xwg->end=1;
	screen.end=1;

	printf("connection input close\n");
}
