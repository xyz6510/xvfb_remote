
int flip(int *v,int a)
{
	if ((*v==0)&&(a==1)) {*v=1;return 0;}
	if ((*v==1)&&(a==0)) {*v=2;return 1;}
	if ((*v==2)&&(a==-1)) {*v=-1;return 0;}
	if ((*v==-1)&&(a==0)) {*v=-2;return 1;}
	if ((*v==-2)&&(a==1)) {*v=1;return 0;}
	if ((*v==0)&&(a==-1)) {*v=-1;return 0;}
	return 0;
}

void command(SRV *srv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	xwg->conn_command=connect_to(srv->str,xwg->ip_addr_str,xwg->ip_addr_num,xwg->ip_port_num+srv->p);

	int e=0;
	while (xwg->end==0) {
		int cmd;
		if (flip(&xwg->command_active_encoder,0)) {
			cmd=0;
			if (xwg->command_active_encoder==2) cmd=1;
			if (xwg->command_active_encoder==-2) cmd=2;
			if (cmd) {e=write_data(xwg->conn_command,&cmd,4,NULL);if (e<=0) break;}
		}
		if (flip(&xwg->command_set_yuv,0)) {
			cmd=0;
			if (xwg->command_set_yuv==2) cmd=3;
			if (xwg->command_set_yuv==-2) cmd=4;
			if (cmd) {e=write_data(xwg->conn_command,&cmd,4,NULL);if (e<=0) break;}
		}
		if (xwg->command_limit_fps==1) {
			xwg->command_limit_fps=0;
			cmd=5;
			//printf("limit fps:%i\n",cmd);
			e=write_data(xwg->conn_command,&cmd,4,NULL);if (e<=0) break;
		}
		if (xwg->command_set_max_fps==1) {
			xwg->command_set_max_fps=0;
			cmd=6;
			//printf("set max fps:%i\n",cmd);
			e=write_data(xwg->conn_command,&cmd,4,NULL);if (e<=0) break;
			e=write_data(xwg->conn_command,&xwg->srv_max_fps,4,NULL);if (e<=0) break;
		}
		if (xwg->command_set_resolution==1) {
			xwg->command_set_resolution=0;
			cmd=7;
			if (xwg->resolutions.num>0) {
				e=write_data(xwg->conn_command,&cmd,4,NULL);if (e<=0) break;
				int xr=xwg->resolutions.resx[xwg->res_idx];
				int yr=xwg->resolutions.resy[xwg->res_idx];
				e=write_data(xwg->conn_command,&xr,4,NULL);if (e<=0) break;
				e=write_data(xwg->conn_command,&yr,4,NULL);if (e<=0) break;
			}
		}
		if (xwg->command_set_xkmap==1) {
			xwg->command_set_xkmap=0;
			cmd=8;
			if (xwg->xkmaps.num>0) {
				e=write_data(xwg->conn_command,&cmd,4,NULL);if (e<=0) break;
				int param=0;
				if (xwg->xkmaps.variant[xwg->map_idx]) param=1;
				e=write_data(xwg->conn_command,&param,4,NULL);if (e<=0) break;
				int len=strlen(xwg->xkmaps.map[xwg->map_idx]);
				e=write_data(xwg->conn_command,&len,4,NULL);if (e<=0) break;
				e=write_data(xwg->conn_command,xwg->xkmaps.map[xwg->map_idx],len,NULL);if (e<=0) break;
				if (param) {
					int len=strlen(xwg->xkmaps.variant[xwg->map_idx]);
					e=write_data(xwg->conn_command,&len,4,NULL);if (e<=0) break;
					e=write_data(xwg->conn_command,xwg->xkmaps.variant[xwg->map_idx],len,NULL);if (e<=0) break;
				}
			}
		}
		usleep(40000);
	}
	close_socket(xwg->conn_command);
	xwg->end=1;
	screen.end=1;

	printf("connection command close\n");
}
