
void argb2i420(int data[3])
{
	SCR *sc=&screen;

	int yw=x264.in.img.i_stride[0];
	int uw=x264.in.img.i_stride[1];
	int vw=x264.in.img.i_stride[2];
    ARGBToI420(sc->img+(data[1]*sc->row),sc->row,
   		x264.in.img.plane[0]+(data[1]*yw),yw,
   		x264.in.img.plane[1]+(data[2]*uw),uw,
   		x264.in.img.plane[2]+(data[2]*vw),vw,
   	    sc->w,data[0]);
}

void argb2i444(int *data)
{
	SCR *sc=&screen;

	int yw=x264.in.img.i_stride[0];
	int uw=x264.in.img.i_stride[1];
	int vw=x264.in.img.i_stride[2];
    ARGBToI444(sc->img+(data[1]*sc->row),sc->row,
   		x264.in.img.plane[0]+(data[1]*yw),yw,
   		x264.in.img.plane[1]+(data[2]*uw),uw,
   		x264.in.img.plane[2]+(data[2]*vw),vw,
   	    sc->w,data[0]);
}

void enc_yuv()
{
	SCR *sc=&screen;

	int cpu=sc->cpu;
	if (cpu>8) cpu=8;
	if (sc->yuv_threads) {
		if (sc->yuv_threads>8) sc->yuv_threads=8;
		cpu=sc->yuv_threads;
	}

	static int data[16][3];
	static pthread_barrier_t bar0;
	static pthread_barrier_t bar1;
	static int start=0;

	void yuv_conv(int *d)
	{
		SCR *sc=&screen;

		while (sc->end==0) {
			pthread_barrier_wait(&bar0);
			int format=x264.param.i_csp;
			if (format==X264_CSP_I420) argb2i420(d);
			if (format==X264_CSP_I444) argb2i444(d);
			pthread_barrier_wait(&bar1);
		}
	}

	if (start==0) {
		start=1;
		pthread_barrier_init(&bar0,NULL,cpu+1);
		pthread_barrier_init(&bar1,NULL,cpu+1);
		pthread_t yuv_thread;
		pthread_attr_t process_attr;
		pthread_attr_init(&process_attr);
		pthread_attr_setstacksize(&process_attr,64*1024);
		int i;
        for (i=0;i<cpu;i++) {
			pthread_create(&yuv_thread,&process_attr,(void*)yuv_conv,(void*)data[i]);
		}
	}

	struct timeval t1,t2;
	gettimeofday(&t1,NULL);

	int lines=sc->h/cpu;
	int lines_uv=lines/2;
	int format=x264.param.i_csp;
	if (format==X264_CSP_I444) lines_uv=lines;

	int i;
	for (i=0;i<cpu-1;i++) {
		data[i][0]=lines;
		data[i][1]=i*lines;
		data[i][2]=i*lines_uv;
	}
	data[i][0]=sc->h-((cpu-1)*lines);
	data[i][1]=i*lines;
	data[i][2]=i*lines_uv;

	pthread_barrier_wait(&bar0);

	pthread_barrier_wait(&bar1);

	gettimeofday(&t2,NULL);
	double td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);
	sc->stat_yuv_time+=td;
}
