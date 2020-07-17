
void stats(SRV *srv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	xwg->conn_stats=connect_to(srv->str,xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num+srv->p);

	int start=1;
	int e=0;
	while (xwg->end==0) {
		int fps;
		int dmg;
		int kbs;
		int cpu;
		int cmp_time;
		int yuv_time;
		int x264_time;
		int send_time;
		e=read_data(xwg->conn_stats,&fps,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&dmg,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&kbs,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&cpu,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&cmp_time,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&yuv_time,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&x264_time,4,NULL);if (e<=0) break;
		e=read_data(xwg->conn_stats,&send_time,4,NULL);if (e<=0) break;
		int total_time=cmp_time+yuv_time+x264_time+send_time;
		int usage=100.0/(1000000.0/(float)total_time);
		if ((terminal.active)||(start==1)) {
			start=0;
			int i;
			for (i=0;i<terminal.w;i++) {
				term_print_at(i,0," ",0x0e06,0);
			}
			char str[32];
			sprintf(str,"C:%i%%",cpu);term_print_at(0,0,str,0x0106,0);
			sprintf(str,"F:%i",fps);term_print_at(7,0,str,0x0106,0);
			sprintf(str,"D:%i",dmg);term_print_at(12,0,str,0x0106,0);
			sprintf(str,"K:%i",kbs);term_print_at(18,0,str,0x0106,0);
			sprintf(str,"c:%0.2f",(float)cmp_time/1000.0);term_print_at(26,0,str,0x0106,0);
			sprintf(str,"y:%0.2f",(float)yuv_time/1000.0);term_print_at(34,0,str,0x0106,0);
			sprintf(str,"x:%0.2f",(float)x264_time/1000.0);term_print_at(42,0,str,0x0106,0);
			sprintf(str,"s:%0.2f",(float)send_time/1000.0);term_print_at(50,0,str,0x0106,0);
			sprintf(str,"u:%i%%",usage);term_print_at(58,0,str,0x0106,0);
			xwg->terminal_update=1;
		}
	}
	close_socket(xwg->conn_stats);
	xwg->end=1;
	screen.end=1;

	printf("connection stats close\n");
}
