#include "types.h"

#include "socket.c"
#include "damage.c"
#include "input.c"
#include "yuv.c"
#include "x264.c"
#include "cmp.c"
#include "stats.c"
#include "command.c"
#include "cursor.c"
#include "clipboard_change.c"
#include "typed_events.c"
#include "send_clipboard.c"
#include "get_clipboard.c"
#include "parse_cmd.c"

void sig(int s)
{
	SCR *sc=&screen;

	sc->end=1;
	static int sig_cnt=0;
	sig_cnt++;
	if (sig_cnt>1) {
		printf(" ctrl-c exit\n");
		exit(-1);
	}
}

int xerr(Display *d,XErrorEvent *e)
{
	return 0;
}

void get_screen_size(int *w,int *h)
{
	SCR *sc=&screen;

	Window win;
	int px,py;
	unsigned int bw,bpp;

	XGetGeometry(sc->d,sc->root,
		&win,&px,&py,(unsigned int*)w,(unsigned int*)h,&bw,&bpp);
}

void map_xvfb(char *file)
{
	SCR *sc=&screen;

	int f=open(file,O_RDONLY);
	if (f<0) {
		printf("error open xvfb: %s\n",file);
		exit(-1);
	}
	struct stat fs;
	fstat(f,&fs);
	sc->map_size=fs.st_size;
	int align=getpagesize();
	posix_memalign(&sc->xvfb,align,sc->map_size);
	sc->xvfb=mmap(sc->xvfb,sc->map_size,PROT_READ,MAP_SHARED|MAP_FIXED,f,0);
	if (sc->xvfb==MAP_FAILED) {
		printf("error mmap xvfb: %s\n",file);
		exit(-1);
	}
	close(f);
	XWDFileHeader *hd=(XWDFileHeader*)sc->xvfb;
	int start=(hd->header_size>>24)+((hd->ncolors>>8)*sizeof(XWDColor));
	sc->fbw=((hd->pixmap_width&0xff000000)>>24)+(256*((hd->pixmap_width&0x00ff0000)>>16));
	sc->fbh=((hd->pixmap_height&0xff000000)>>24)+(256*((hd->pixmap_height&0x00ff0000)>>16));
	sc->row=sc->fbw*4;
	sc->img=sc->xvfb+start;
}

void open_enc()
{
	SCR *sc=&screen;

	map_xvfb(sc->xvfb_file);
	sc->d=XOpenDisplay(NULL);
	sc->root=DefaultRootWindow(sc->d);
	get_screen_size(&sc->w,&sc->h);
	sc->cpu=sysconf(_SC_NPROCESSORS_ONLN);
	if (sc->format==0) sc->format=2;
	sc->win=XCreateSimpleWindow(sc->d,sc->root,0,0,100,100,0,0,0);
	sc->clip_data=malloc(64*1024);
	sc->clip_size=0;
	sc->UTF8=XInternAtom(sc->d,"UTF8_STRING",True);
	sc->UTF8_1=XInternAtom(sc->d,"text/plain;charset=utf-8",True);
	sc->TARGETS=XInternAtom(sc->d,"TARGETS",False);
	sc->limit_fps_def=25;
}

void close_enc()
{
	SCR *sc=&screen;

	x264_close();
	munmap(sc->xvfb,sc->map_size);
	XDestroyWindow(sc->d,sc->win);
	XCloseDisplay(sc->d);
	free(sc->clip_data);
}

int main(int argc,char **argv)
{
	SCR *sc=&screen;
	memset(&screen,0,sizeof(SCR));

	signal(SIGINT,sig);
	signal(SIGPIPE,SIG_IGN);

	parse_cmd(argc,argv);

	XInitThreads();
	XSetErrorHandler(xerr);

	open_enc();

	printf("fb:%ix%i res:%ix%i\n",sc->fbw,sc->fbh,sc->w,sc->h);

	pthread_attr_t process_attr;
	pthread_attr_init(&process_attr);
	pthread_attr_setstacksize(&process_attr,64*1024);

	pthread_t damage_thread;
	pthread_create(&damage_thread,&process_attr,(void*)&damage,NULL);

	SRV input_data={"input",1};
	pthread_t input_thread;
	pthread_create(&input_thread,&process_attr,(void*)&input,&input_data);

	SRV stats_data={"stats",2};
	pthread_t stats_thread;
	pthread_create(&stats_thread,&process_attr,(void*)&stats,&stats_data);

	SRV command_data={"command",3};
	pthread_t command_thread;
	pthread_create(&command_thread,&process_attr,(void*)&command,&command_data);

	SRV cursor_data={"cursor",4};
	pthread_t cursor_thread;
	pthread_create(&cursor_thread,&process_attr,(void*)&cursor,&cursor_data);

	pthread_t clipboard_change_thread;
	pthread_create(&clipboard_change_thread,&process_attr,(void*)&clipboard_change,NULL);

	SRV clip_send_data={"clipboard send",5};
	pthread_t send_clipboard_thread;
	pthread_create(&send_clipboard_thread,&process_attr,(void*)&send_clipboard,&clip_send_data);

	SRV clip_get_data={"clipboard get",6};
	pthread_t get_clipboard_thread;
	pthread_create(&get_clipboard_thread,&process_attr,(void*)&get_clipboard,&clip_get_data);

	pthread_t typed_events_thread;
	pthread_create(&typed_events_thread,&process_attr,(void*)&typed_events,NULL);

	int max_fps=60;
	int limit_fps_old;
	int limit_fps;

	int sck=create_listen_socket(sc->ip_addr_num,sc->ip_port_num);
	int client_socket;
l1:
	client_socket=accept_connection(sck,"x264",sc->ip_addr_str,sc->ip_port_num);

	get_screen_size(&sc->w,&sc->h);
	write_data(client_socket,&sc->fbw,4,NULL);
	write_data(client_socket,&sc->fbh,4,NULL);
l2:
	x264_close();
	x264_open(sc->format);
	write_data(client_socket,&sc->w,4,NULL);
	write_data(client_socket,&sc->h,4,NULL);
	write_data(client_socket,&sc->format,4,NULL);
	write_data(client_socket,&sc->limit_fps_def,4,NULL);

	limit_fps=sc->limit_fps_def;
	limit_fps_old=limit_fps;
	int size=0;
	int start=15;
	int dmg_old=0;
	int dmg_cnt=-1;
	int res_change=0;
	int live_cnt=0;
	int fps_count=0;
	int res_x,res_y;
	get_screen_size(&res_x,&res_y);
	while (sc->end==0) {
		struct timeval t1,t2;
		gettimeofday(&t1,NULL);

		res_change++;
		int format=0;
		if (res_change>6) {
			res_change=0;
			get_screen_size(&res_x,&res_y);
		}
		if (sc->command_set_yuv420) {
			sc->command_set_yuv420=0;
			format=1;
			sc->format=2;
		}
		if (sc->command_set_yuv444) {
			sc->command_set_yuv444=0;
			format=1;
			sc->format=1;
		}
		if (sc->command_limit_fps==1) {
			sc->command_limit_fps=0;
			if (limit_fps>15) {
				if (fps_count==0) {
					limit_fps_old=limit_fps;
					limit_fps=15;
					fps_count=15;
				}
			}
		}
		if (sc->command_set_max_fps==1) {
			sc->command_set_max_fps=0;
			if (limit_fps_old!=sc->limit_fps_def) {
				limit_fps_old=sc->limit_fps_def;
				if (fps_count==0) limit_fps=sc->limit_fps_def;
				//printf("limit fps:%i\n",limit_fps);
			} else {
				limit_fps=sc->limit_fps_def;
			}
		}
		if ((res_x!=sc->w)||(res_y!=sc->h)||(format)) {
			sc->w=res_x;
			sc->h=res_y;
			//printf("format:%i\n",sc->format);
			int type=60;
			write_data(client_socket,&type,4,NULL);
			goto l2;
		}
		if (sc->dmg_cnt) {
			//printf("dmg:%i\n",sc->dmg_cnt);
			dmg_cnt=2;
			if (dmg_old!=0) dmg_old=0;
			else dmg_old=sc->dmg_cnt;
			sc->dmg_cnt=0;
		}
		if (sc->command_active_encoder==1) {
			sc->command_active_encoder=2;
			start=5;
		}
		int x264_write=0;
		dmg_cnt--;
		if (dmg_cnt<0) dmg_cnt=0;
		if ((dmg_cnt==0)&&(start==0)) goto skip;
		if (sc->command_active_encoder==0) goto skip;
		if (cmp_img()||(start)) {
			start--;
			if (start<0) start=0;
			x264_write=1;
			enc_yuv();
			size=x264_encode();
			struct timeval t1,t2;
			gettimeofday(&t1,NULL);
			int type=55;
			write_data(client_socket,&type,4,NULL);
			write_data(client_socket,&size,4,NULL);
			int e=write_data(client_socket,x264.nals->p_payload,size,NULL);
			sc->stat_data_send+=size;
			sc->stat_fps_cnt++;
			gettimeofday(&t2,NULL);
			double td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);
			sc->stat_send_time+=td;
			if (e<=0) goto sck_err;
		}
skip:
		if (x264_write) goto skip1;
		live_cnt++;
		if (live_cnt>(100/(1000/limit_fps))) {
			live_cnt=0;
			int type=33;
			int e=write_data(client_socket,&type,4,NULL);
			if (e<=0) {
sck_err:
				close_socket(client_socket);
				dmg_cnt=-1;
				dmg_old=0;
				goto l1;
			}
		}
skip1:
		gettimeofday(&t2,NULL);
		double td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);
		int usl=(16000*((max_fps*10)/limit_fps))/10;
		usl=usl-td;
		if (usl<0) usl=0;
		usleep(usl);
		if ((sc->dmg_cnt==0)&&(dmg_old!=0)) {
			sc->dmg_cnt=1;
		}
		if (fps_count>0) {
			fps_count--;
			if (fps_count==0) {
				fps_count=0;
				limit_fps=limit_fps_old;
			}
		}
	}

	close_socket(sck);

	pthread_join(input_thread,NULL);
	pthread_join(stats_thread,NULL);
	pthread_join(command_thread,NULL);
	pthread_join(cursor_thread,NULL);
	pthread_join(send_clipboard_thread,NULL);
	pthread_join(get_clipboard_thread,NULL);

	close_enc();

	return 0;
}
