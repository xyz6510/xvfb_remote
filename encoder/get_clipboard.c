
void get_clipboard(SRV *srv)
{
	SCR *sc=&screen;

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num+srv->p);
	int client_socket;
l1:
	client_socket=accept_connection(sck,srv->str,sc->ip_addr_str,sc->ip_port_num+srv->p);

	int e=0;
	while (sc->end==0) {
		int size;
		e=read_data(client_socket,&size,4,NULL);if (e<=0) break;
		e=read_data(client_socket,sc->clip_data,size,NULL);if (e<=0) break;
		sc->clip_data[size]=0;
		sc->clip_size=size;
		//printf("get clipboard:[%s]\n",sc->clip_data);
		XSetSelectionOwner(sc->d,XA_PRIMARY,sc->win,CurrentTime);
		if (XGetSelectionOwner(sc->d,XA_PRIMARY)!=sc->win) {
			printf("set selection owner fail\n");
		}
	}
	printf("connection get clipboard closed\n");
	close_socket(client_socket);
	if (sc->end==0) goto l1;
	printf("get clipboard exit\n");
}
