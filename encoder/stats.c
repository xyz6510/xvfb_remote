
void stats(SRV *srv)
{
	SCR *sc=&screen;

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num+srv->p);
	int client_socket;
l1:
	client_socket=accept_connection(sck,srv->str,sc->ip_addr_str,sc->ip_port_num+srv->p);

	int e=0;
	while (sc->end==0) {
		struct rusage usage1,usage2;

		getrusage(RUSAGE_SELF,&usage1);

		usleep(1000*1000);

		getrusage(RUSAGE_SELF,&usage2);

		double t1=(usage1.ru_utime.tv_sec*1000000)+(usage1.ru_stime.tv_sec*1000000)+(usage1.ru_utime.tv_usec)+(usage1.ru_stime.tv_usec);
		double t2=(usage2.ru_utime.tv_sec*1000000)+(usage2.ru_stime.tv_sec*1000000)+(usage2.ru_utime.tv_usec)+(usage2.ru_stime.tv_usec);
		double td=t2-t1;
		td=100.0/(1000000.0/td);

		sc->stat_cpu_usage=td;

		sc->stat_fps=sc->stat_fps_cnt;
		sc->stat_fps_cnt=0;
		sc->stat_dmg=sc->stat_dmg_cnt2;
		sc->stat_dmg_cnt2=0;
		sc->stat_data_send_kb=sc->stat_data_send/1024;
		sc->stat_data_send=0;
		int cmp_time=sc->stat_cmp_time;
		int yuv_time=sc->stat_yuv_time;
		int x264_time=sc->stat_x264_time;
		int send_time=sc->stat_send_time;
		sc->stat_cmp_time=0;
		sc->stat_yuv_time=0;
		sc->stat_x264_time=0;
		sc->stat_send_time=0;
		e=write_data(client_socket,&sc->stat_fps,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&sc->stat_dmg,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&sc->stat_data_send_kb,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&sc->stat_cpu_usage,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&cmp_time,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&yuv_time,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&x264_time,4,NULL);if (e<=0) break;
		e=write_data(client_socket,&send_time,4,NULL);if (e<=0) break;
	}
	printf("connection stats closed\n");
	close_socket(client_socket);
	if (sc->end==0) goto l1;
	printf("stats exit\n");
}
