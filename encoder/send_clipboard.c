
void send_clipboard(SRV *srv)
{
	SCR *sc=&screen;

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num+srv->p);
	int client_socket;
l1:
	client_socket=accept_connection(sck,srv->str,sc->ip_addr_str,sc->ip_port_num+srv->p);

	int live_cnt=0;
	int e=0;
	while (sc->end==0) {
		if (sc->send_clipboard_data==1) {
			sc->send_clipboard_data=0;
			int type=92;
			int size=sc->clip_size;
			e=write_data(client_socket,&type,4,NULL);if (e<=0) break;
			e=write_data(client_socket,&size,4,NULL);if (e<=0) break;
			e=write_data(client_socket,sc->clip_data,size,NULL);if (e<=0) break;
			sc->clip_size=size;
		} else {
			live_cnt++;
			if (live_cnt>3) {
				live_cnt=0;
				int type=46;
				e=write_data(client_socket,&type,4,NULL);if (e<=0) break;
			}
		}
		usleep(40000);
	}
	printf("connection send clipboard closed\n");
	close_socket(client_socket);
	if (sc->end==0) goto l1;
	printf("send clipboard exit\n");
}
