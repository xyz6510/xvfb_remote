
void input(SRV *srv)
{
	SCR *sc=&screen;

	Display *disp=XOpenDisplay(NULL);

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num+srv->p);
	int client_socket;
l1:
	client_socket=accept_connection(sck,srv->str,sc->ip_addr_str,sc->ip_port_num+srv->p);

	int e=0;
	while (sc->end==0) {
		int type;
		e=read_data(client_socket,&type,4,NULL);if (e<=0) break;
//		printf("type:%i\n",type);
		if (type==1) {
			int x,y;
			e=read_data(client_socket,&x,4,NULL);if (e<=0) break;
			e=read_data(client_socket,&y,4,NULL);if (e<=0) break;
			XTestFakeMotionEvent(disp,-1,x,y,CurrentTime);
		}
		if (type==2) {
			int button;
			int state;
			e=read_data(client_socket,&button,4,NULL);if (e<=0) break;
			e=read_data(client_socket,&state,4,NULL);if (e<=0) break;
			XTestFakeButtonEvent(disp,button,state,CurrentTime);
			if ((button==4)||(button==5)) {
				XTestFakeButtonEvent(disp,button,0,CurrentTime);
			}
		}
		if (type==3) {
			int key;
			int state;
			e=read_data(client_socket,&key,4,NULL);if (e<=0) break;
			e=read_data(client_socket,&state,4,NULL);if (e<=0) break;
			if (state>0) XTestFakeKeyEvent(disp,key,True,CurrentTime);
			if (state<0) XTestFakeKeyEvent(disp,key,False,CurrentTime);
		}
		XSync(disp,False);
	}
	printf("connection input closed\n");
	close_socket(client_socket);
	if (sc->end==0) goto l1;
	printf("input exit\n");
}
