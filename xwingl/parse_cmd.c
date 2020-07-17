
void print_usage()
{
	printf("\n");
	printf("usage: xwingl -ip ipaddr -p port [ -res WxH ] [ -map xx[:yy] ] [ -img n.m,n.m,n.m,n.m ] [ -zoom x,y,w,h ] [ -cscale n ] [ -font n ] [ -term n ] [ -img_idx n ] \n");
	printf("       -res WxH :resolution to add\n");
	printf("       -map xx[:yy] :setxkbmap xx -variant yy\n");
	printf("       -img n.m,n.m,n.m,n.m :gamma,saturation,brightness,contrast\n");
	printf("       -zoom x,y,w,h :zoom part of screen\n");
	printf("       -cscale n :cursor scale 1-8 0-2\n");
	printf("       -font n :terminal font size 8,16,24,32,40 0-16\n");
	printf("       -term n :terminal show on start 1 show 0 hide\n");
	printf("       -img_idx n :default img index on start\n");
	printf("example: xwingl -ip 127.0.0.1 -p 33500 -res 1024x576 -res 800x600 -map us -map us:dvorak -img 1.0,0.0,0.0,0.0 -img 1.0,0.2,-0.07,-0.12 -zoom 0,200,400,300 -cscale 1 -font 8 -term 1 -img_idx 1\n");
	printf("\n");

	exit(-1);
}

int add_resolution(char *res)
{
	XWINGL_VAR *xwg=&xwingl_var;

	char xres[16];
	char yres[16];
	int xr=0;
	int yr=0;
	memset(xres,0,16);
	memset(yres,0,16);
	int len=strlen(res);
	if (len>=16) return -1;
	int pos=-1;
	int i;
	for (i=0;i<len;i++) {
		if (res[i]=='x') {
			pos=i;
			break;
		}
	}
	if (pos<0) return -1;
	if ((pos>0)&&(pos<len-1)&&(pos<16)&&(len-pos<16)) {
		strncpy(xres,res,pos);
		strncpy(yres,&res[pos+1],len-pos);
		xr=atoi(xres);
		yr=atoi(yres);
		if ((xr==0)||(yr==0)) return -1;
		for (i=0;i<xwg->resolutions.num;i++) {
			if ((xwg->resolutions.resx[i]==xr)&&(xwg->resolutions.resy[i]==yr)) return 0;
		}
		xwg->resolutions.resx[xwg->resolutions.num]=xr;
		xwg->resolutions.resy[xwg->resolutions.num]=yr;
		xwg->resolutions.num++;
		if (xwg->resolutions.num>=xwg->resolutions.max) {
			xwg->resolutions.max+=xwg->resolutions.max;
			xwg->resolutions.resx=realloc(xwg->resolutions.resx,xwg->resolutions.max*4);
			xwg->resolutions.resy=realloc(xwg->resolutions.resy,xwg->resolutions.max*4);
			if ((xwg->resolutions.resx==NULL)||(xwg->resolutions.resy==NULL)) return -1;
		}
	} else return -1;

	return 0;
}

int add_xkbmap(char *xk)
{
	XWINGL_VAR *xwg=&xwingl_var;

	char map[32];
	char variant[32];
	int var=0;
	memset(map,0,32);
	memset(variant,0,32);
	int len=strlen(xk);
	if (len>=32) return -1;
	int pos=-1;
	int i;
	for (i=0;i<len;i++) {
		if (xk[i]==':') {
			pos=i;
			var=1;
			break;
		}
	}
	if (var) {
		if ((pos>0)&&(pos<len-1)&&(pos<32)&&(len-pos<32)) {
			strncpy(map,xk,pos);
			strncpy(variant,&xk[pos+1],len-pos);
		} else return -1;
	} else {
		strncpy(map,xk,len);
	}
	for (i=0;i<xwg->xkmaps.num;i++) {
		int cmp=strcmp(xwg->xkmaps.map[i],map);
		if ((cmp==0)&&(var==0)) return 0;
		else if ((cmp==0)&&(var)) {
			if (xwg->xkmaps.variant[i]!=NULL) {
				if (strcmp(xwg->xkmaps.variant[i],variant)==0) return 0;
			}
		}
	}
	xwg->xkmaps.map[xwg->xkmaps.num]=strdup(map);
	if (var) xwg->xkmaps.variant[xwg->xkmaps.num]=strdup(variant);
	else xwg->xkmaps.variant[xwg->xkmaps.num]=NULL;
	xwg->xkmaps.num++;
	if (xwg->xkmaps.num>=xwg->xkmaps.max) {
		xwg->xkmaps.max+=xwg->xkmaps.max;
		xwg->xkmaps.map=realloc(xwg->xkmaps.map,xwg->xkmaps.max*(sizeof(char*)));
		xwg->xkmaps.variant=realloc(xwg->xkmaps.variant,xwg->xkmaps.max*(sizeof(char*)));
		if ((xwg->xkmaps.map==NULL)||(xwg->xkmaps.variant==NULL)) return -1;
	}

	return 0;
}

int add_img_settings(char *img)
{
	XWINGL_VAR *xwg=&xwingl_var;

	int len=strlen(img);
	if (len>=32) return -1;

	float gsbc[4];;
	int num=0;
	int pos=0;
	char fl[32];
	int i,j,k;
	for (i=0;i<len;i++) {
		if (img[i]==',') {
			int k=0;
			for (j=pos;j<i;j++) {
				fl[k]=img[j];
				k++;
			}
			fl[k]=0;
			gsbc[num]=atof(fl);
			num++;
			pos=i+1;
		}
	}
	k=0;
	for (j=pos;j<len;j++) {
		fl[k]=img[j];
		k++;
	}
	fl[k]=0;
	gsbc[num]=atof(fl);
	num++;

	if (num!=4) return -1;

	for (i=0;i<xwg->image.num;i++) {
		if ((xwg->image.g[i]==gsbc[0])&&(xwg->image.s[i]==gsbc[1])&&
			(xwg->image.b[i]==gsbc[2])&&(xwg->image.c[i]==gsbc[3])) return 0;
	}

	xwg->image.g[xwg->image.num]=gsbc[0];
	xwg->image.s[xwg->image.num]=gsbc[1];
	xwg->image.b[xwg->image.num]=gsbc[2];
	xwg->image.c[xwg->image.num]=gsbc[3];

	xwg->image.num++;
	if (xwg->image.num>=xwg->image.max) {
		xwg->image.max+=xwg->image.max;
		xwg->image.g=realloc(xwg->image.g,xwg->image.max*(sizeof(float)));
		xwg->image.s=realloc(xwg->image.s,xwg->image.max*(sizeof(float)));
		xwg->image.b=realloc(xwg->image.b,xwg->image.max*(sizeof(float)));
		xwg->image.c=realloc(xwg->image.c,xwg->image.max*(sizeof(float)));
		if ((xwg->image.g==NULL)||(xwg->image.s==NULL)||
			(xwg->image.b==NULL)||(xwg->image.c==NULL)) return -1;
	}

	return 0;
}

int add_zoom_settings(char *zoom)
{
	XWINGL_VAR *xwg=&xwingl_var;

	int len=strlen(zoom);
	if (len>=32) return -1;
	int xywh[4];;
	int num=0;
	int pos=0;
	char inum[32];
	int i,j,k;
	for (i=0;i<len;i++) {
		if (zoom[i]==',') {
			int k=0;
			for (j=pos;j<i;j++) {
				inum[k]=zoom[j];
				k++;
			}
			inum[k]=0;
			xywh[num]=atoi(inum);
			num++;
			pos=i+1;
		}
	}
	k=0;
	for (j=pos;j<len;j++) {
		inum[k]=zoom[j];
		k++;
	}
	inum[k]=0;
	xywh[num]=atoi(inum);
	num++;

	if (num!=4) return -1;

	for (i=0;i<4;i++) {
		if (xywh[i]<0) return -1;
	}

	for (i=0;i<xwg->zoom.num;i++) {
		if ((xwg->zoom.x[i]==xywh[0])&&(xwg->zoom.y[i]==xywh[1])&&
			(xwg->zoom.w[i]==xywh[2])&&(xwg->zoom.h[i]==xywh[3])) return 0;
	}

	xwg->zoom.x[xwg->zoom.num]=xywh[0];
	xwg->zoom.y[xwg->zoom.num]=xywh[1];
	xwg->zoom.w[xwg->zoom.num]=xywh[2];
	xwg->zoom.h[xwg->zoom.num]=xywh[3];

	xwg->zoom.num++;
	if (xwg->zoom.num>=xwg->zoom.max) {
		xwg->zoom.max+=xwg->zoom.max;
		xwg->zoom.x=realloc(xwg->zoom.x,xwg->zoom.max*(sizeof(float)));
		xwg->zoom.y=realloc(xwg->zoom.y,xwg->zoom.max*(sizeof(float)));
		xwg->zoom.w=realloc(xwg->zoom.w,xwg->zoom.max*(sizeof(float)));
		xwg->zoom.h=realloc(xwg->zoom.h,xwg->zoom.max*(sizeof(float)));
		if ((xwg->zoom.x==NULL)||(xwg->zoom.y==NULL)||
			(xwg->zoom.w==NULL)||(xwg->zoom.h==NULL)) return -1;
	}

	return 0;
}

void parse_cmd(int argc,char **argv)
{
	XWINGL_VAR *xwg=&xwingl_var;

	xwg->resolutions.num=0;
	xwg->resolutions.max=16;
	xwg->resolutions.resx=malloc(xwg->resolutions.max*4);
	xwg->resolutions.resy=malloc(xwg->resolutions.max*4);
	memset(xwg->resolutions.resx,0,xwg->resolutions.max*4);
	memset(xwg->resolutions.resy,0,xwg->resolutions.max*4);

	xwg->xkmaps.num=0;
	xwg->xkmaps.max=16;
	xwg->xkmaps.map=malloc(xwg->xkmaps.max*sizeof(char*));
	xwg->xkmaps.variant=malloc(xwg->xkmaps.max*sizeof(char*));
	memset(xwg->xkmaps.map,0,xwg->xkmaps.max*sizeof(char*));
	memset(xwg->xkmaps.variant,0,xwg->xkmaps.max*sizeof(char*));

	xwg->image.num=0;
	xwg->image.max=16;
	xwg->image.g=malloc(xwg->image.max*sizeof(float));
	xwg->image.s=malloc(xwg->image.max*sizeof(float));
	xwg->image.b=malloc(xwg->image.max*sizeof(float));
	xwg->image.c=malloc(xwg->image.max*sizeof(float));
	memset(xwg->image.g,0,xwg->image.max*sizeof(float));
	memset(xwg->image.s,0,xwg->image.max*sizeof(float));
	memset(xwg->image.b,0,xwg->image.max*sizeof(float));
	memset(xwg->image.c,0,xwg->image.max*sizeof(float));

	xwg->zoom.num=0;
	xwg->zoom.max=16;
	xwg->zoom.x=malloc(xwg->zoom.max*4);
	xwg->zoom.y=malloc(xwg->zoom.max*4);
	xwg->zoom.w=malloc(xwg->zoom.max*4);
	xwg->zoom.h=malloc(xwg->zoom.max*4);
	memset(xwg->zoom.x,0,xwg->zoom.max*4);
	memset(xwg->zoom.y,0,xwg->zoom.max*4);
	memset(xwg->zoom.w,0,xwg->zoom.max*4);
	memset(xwg->zoom.h,0,xwg->zoom.max*4);

	int ip=0;
	int port=0;

	int i;
	for (i=1;i<argc;i++) {
		if (strcmp("-ip",argv[i])==0) {
			if (++i<argc) {
				xwg->ip_addr_num=inet_addr(argv[i]);
				if (xwg->ip_addr_num==INADDR_NONE) {
					printf("invalid ip:%s\n",argv[i]);
					print_usage();
				}
				if (xwg->ip_addr_str) free(xwg->ip_addr_str);
				xwg->ip_addr_str=strdup(argv[i]);
				ip=1;
				//printf("ip:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-p",argv[i])==0) {
			if (++i<argc) {
				xwg->ip_port_num=atoi(argv[i]);
				if (xwg->ip_port_num==0) {
					printf("invalid port:%s\n",argv[i]);
					print_usage();
				}
				if (xwg->ip_port_str) free(xwg->ip_port_str);
				xwg->ip_port_str=strdup(argv[i]);
				port=1;
				//printf("port:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-res",argv[i])==0) {
			if (++i<argc) {
				if (add_resolution(argv[i])) {
					printf("invalid resolution:%s\n",argv[i]);
					print_usage();
				}
				//printf("res:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-map",argv[i])==0) {
			if (++i<argc) {
				if (add_xkbmap(argv[i])) {
					printf("invalid xkbmap:%s\n",argv[i]);
					print_usage();
				}
				//printf("xk:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-img",argv[i])==0) {
			if (++i<argc) {
				if (add_img_settings(argv[i])) {
					printf("invalid image settings:%s\n",argv[i]);
					print_usage();
				}
				//printf("image:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-zoom",argv[i])==0) {
			if (++i<argc) {
				if (add_zoom_settings(argv[i])) {
					printf("invalid zoom settings:%s\n",argv[i]);
					print_usage();
				}
				//printf("zoom:%s\n",argv[i]);
			}
			continue;
		}
		if (strcmp("-cscale",argv[i])==0) {
			if (++i<argc) {
				int cscale=atoi(argv[i]);
				if ((cscale<0)||(cscale>8)) {
					printf("invalid cursor scale:%s\n",argv[i]);
					print_usage();
				}
				xwg->cursor_scale=cscale;
			}
			continue;
		}
		if (strcmp("-font",argv[i])==0) {
			if (++i<argc) {
				int font=atoi(argv[i]);
				if ((font<0)||(font>40)||((font/8)*8!=font)) {
					printf("invalid font size:%s\n",argv[i]);
					print_usage();
				}
				xwg->font_size=font;
			}
			continue;
		}
		if (strcmp("-term",argv[i])==0) {
			if (++i<argc) {
				int term=atoi(argv[i]);
				if ((term<0)||(term>1)) {
					printf("invalid terminal active:%s\n",argv[i]);
					print_usage();
				}
				xwg->term_active=term;
			}
			continue;
		}
		if (strcmp("-img_idx",argv[i])==0) {
			if (++i<argc) {
				int idx=atoi(argv[i]);
				if ((idx<0)||(idx>255)) {
					printf("invalid image index:%s\n",argv[i]);
					print_usage();
				}
				xwg->img_idx=idx;
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
}
