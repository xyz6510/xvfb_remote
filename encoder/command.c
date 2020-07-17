
void command(SRV *srv)
{
	SCR *sc=&screen;

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num+srv->p);
	int client_socket;
l1:
	client_socket=accept_connection(sck,srv->str,sc->ip_addr_str,sc->ip_port_num+srv->p);

	int e=0;
	while (sc->end==0) {
		int cmd;
		e=read_data(client_socket,&cmd,4,NULL);if (e<=0) break;
		if (cmd==1) {
			sc->command_active_encoder=1;
		}
		if (cmd==2) {
			sc->command_active_encoder=0;
		}
		if (cmd==3) {
			sc->command_set_yuv420=1;
		}
		if (cmd==4) {
			sc->command_set_yuv444=1;
		}
		if (cmd==5) {
			sc->command_limit_fps=1;
		}
		if (cmd==6) {
			e=read_data(client_socket,&sc->limit_fps_def,4,NULL);if (e<=0) break;
			sc->command_set_max_fps=1;
		}
		if (cmd==7) {
			int xr,yr;
			e=read_data(client_socket,&xr,4,NULL);if (e<=0) break;
			e=read_data(client_socket,&yr,4,NULL);if (e<=0) break;
			char str[255];
			sprintf(str,"xrandr --newmode \"%ix%i\" 0 %i 0 0 0 %i 0 0 0 > /dev/null 2>&1",xr,yr,xr,yr);
			system(str);
			sprintf(str,"xrandr --addmode screen \"%ix%i\" > /dev/null 2>&1",xr,yr);
			system(str);
			sprintf(str,"xrandr -s %ix%i > /dev/null 2>&1",xr,yr);
			system(str);
		}
		if (cmd==8) {
			int param;
			e=read_data(client_socket,&param,4,NULL);if (e<=0) break;
			int size;
			char map[32];
			char variant[32];
			char str[255];
			e=read_data(client_socket,&size,4,NULL);if (e<=0) break;
			e=read_data(client_socket,map,size,NULL);if (e<=0) break;
			map[size]=0;
			if (param) {
				e=read_data(client_socket,&size,4,NULL);if (e<=0) break;
				e=read_data(client_socket,variant,size,NULL);if (e<=0) break;
				variant[size]=0;
			}
			if (param) {
				sprintf(str,"setxkbmap %s -variant %s > /dev/null 2>&1",map,variant);
			} else sprintf(str,"setxkbmap %s > /dev/null 2>&1",map);
			system(str);
		}
	}
	printf("connection command closed\n");
	close_socket(client_socket);
	if (sc->end==0) goto l1;
	printf("command exit\n");
}
