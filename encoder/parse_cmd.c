
void print_usage()
{
	printf("\n");
	printf("usage: DISPLAY=:nn encoder -ip ipaddr -p port -xm Xvfb_framebuffers_file [ -yuv nnn ] [ -ct n ] [ -yt n ] [ -xt n ]\n");
	printf("       DISPLAY=:nn :display where Xvfb is running\n");
	printf("       -yuv nnn :yuv format on start 420 or 444 can be changed from client\n");
	printf("       -ct n :number of image compare threads 1-8 0-number of cpu's\n");
	printf("       -yt n :number of argb to yuv threads 1-8 0-number of cpu's\n");
	printf("       -xt n :number of x264 encoder threads 1-8 0-number of cpu's /2\n");
	printf("example: DISPLAY=:15 encoder -ip 127.0.0.1 -p 33500 -xm /run/Xvfb_screen0 -yuv 444 -ct 1 -yt 1 -xt 1\n");
	printf("\n");

	exit(-1);
}

void parse_cmd(int argc,char **argv)
{
	SCR *sc=&screen;

	int ip=0;
	int port=0;
	int map=0;
	int i;
	for (i=1;i<argc;i++) {
		if (strcmp("-ip",argv[i])==0) {
			if (++i<argc) {
				sc->ip_addr_num=inet_addr(argv[i]);
				if (sc->ip_addr_num==INADDR_NONE) {
					printf("invalid ip:%s\n",argv[i]);
					print_usage();
				}
				if (sc->ip_addr_str) free(sc->ip_addr_str);
				sc->ip_addr_str=strdup(argv[i]);
				ip=1;
				//printf("ip:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-p",argv[i])==0) {
			if (++i<argc) {
				sc->ip_port_num=atoi(argv[i]);
				if (sc->ip_port_num==0) {
					printf("invalid port:%s\n",argv[i]);
					print_usage();
				}
				if (sc->ip_port_str) free(sc->ip_port_str);
				sc->ip_port_str=strdup(argv[i]);
				port=1;
				//printf("port:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-xm",argv[i])==0) {
			if (++i<argc) {
				if (sc->xvfb_file) free(sc->xvfb_file);
				sc->xvfb_file=strdup(argv[i]);
				map=1;
				//printf("xmap:%s\n",argv[i]);
			} else {
				printf("no xvfb file\n");
				print_usage();
			}
			continue;
		}
		if (strcmp("-yuv",argv[i])==0) {
			if (++i<argc) {
				if (strcmp(argv[i],"420")==0) sc->format=2;
				else if (strcmp(argv[i],"444")==0) sc->format=1;
				else {
					printf("invalid format:%s\n",argv[i]);
					print_usage();
				}
			}
			continue;
		}
		if (strcmp("-ct",argv[i])==0) {
			if (++i<argc) {
				int thr=atoi(argv[i]);
				if ((thr<0) || (thr>8)) {
					printf("invalid number of cmp threads:%s\n",argv[i]);
					print_usage();
				} else {
					sc->cmp_threads=thr;
				}
			}
			continue;
		}
		if (strcmp("-yt",argv[i])==0) {
			if (++i<argc) {
				int thr=atoi(argv[i]);
				if ((thr<0) || (thr>8)) {
					printf("invalid number of yuv threads:%s\n",argv[i]);
					print_usage();
				} else {
					sc->yuv_threads=thr;
				}
			}
			continue;
		}
		if (strcmp("-xt",argv[i])==0) {
			if (++i<argc) {
				int thr=atoi(argv[i]);
				if ((thr<0) || (thr>8)) {
					printf("invalid number of x264 threads:%s\n",argv[i]);
					print_usage();
				} else {
					sc->x264_threads=thr;
				}
			}
			continue;
		}
	}
	if (ip==0) {
		printf("no ip address\n");
		print_usage();
	}
	if (port==0) {
		printf("no port\n");
		print_usage();
	}
	if (map==0) {
		printf("no xvfb file\n");
		print_usage();
	}
}
