
int cmp_img()
{
	SCR *sc=&screen;

	int cpu=sc->cpu;
	if (cpu>8) cpu=8;
	if (sc->cmp_threads) {
		if (sc->cmp_threads>8) sc->cmp_threads=8;
		cpu=sc->cmp_threads;
	}

	static uint64_t *line_crc_n=NULL;
	static uint64_t *line_crc_o=NULL;
	static int num_lines=0;
	if (num_lines!=sc->h) {
		if (line_crc_n) free(line_crc_n);
		if (line_crc_o) free(line_crc_o);
		num_lines=sc->h;
		line_crc_n=malloc(num_lines*8);
		line_crc_o=malloc(num_lines*8);
		memset(line_crc_n,0,num_lines*8);
		memset(line_crc_o,0,num_lines*8);
	}

	static int data[16][2];
	static pthread_barrier_t bar0;
	static pthread_barrier_t bar1;
	static int start=0;

	void crc_calc(int *d)
	{
		SCR *sc=&screen;

		while (sc->end==0) {
			pthread_barrier_wait(&bar0);

			int start=d[0];
			int end=d[1];
			int w=sc->w/2;
			int fw=sc->fbw/2;
			uint64_t (*buf)[fw]=(uint64_t(*)[fw]) sc->img;

			int i,j;
			for (i=start;i<end;i+=2) {
				uint64_t crc=0;
				for (j=0;j<w;j++) {
					crc=_mm_crc32_u64(crc,buf[i][j]);
					//crc+=buf[i][j];
				}
				line_crc_n[i]=crc;
			}

			pthread_barrier_wait(&bar1);
		}
	}

	if (start==0) {
		start=1;
		pthread_barrier_init(&bar0,NULL,cpu+1);
		pthread_barrier_init(&bar1,NULL,cpu+1);
		pthread_t crc_thread;
		pthread_attr_t process_attr;
		pthread_attr_init(&process_attr);
		pthread_attr_setstacksize(&process_attr,64*1024);
		int i;
		for (i=0;i<cpu;i++) {
			pthread_create(&crc_thread,&process_attr,(void*)crc_calc,(void*)data[i]);
		}
	}

	struct timeval t1,t2;
	gettimeofday(&t1,NULL);

	int lines=sc->h/cpu;

	int i;
	for (i=0;i<cpu;i++) {
		data[i][0]=i*lines;
		data[i][1]=(i*lines)+lines;
	}

	pthread_barrier_wait(&bar0);

	pthread_barrier_wait(&bar1);

	int ret=0;
	for (i=0;i<sc->h;i++) {
		if (line_crc_n[i]!=line_crc_o[i]) ret=1;
		line_crc_o[i]=line_crc_n[i];
	}

	gettimeofday(&t2,NULL);
	double td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);
	sc->stat_cmp_time+=td;

	return ret;
}
