
void x264_open(int yuv_format)
{
	SCR *sc=&screen;

	int cpu=sc->cpu;
	if (cpu>8) cpu=8;
	if (sc->x264_threads) {
		if (sc->x264_threads>8) sc->x264_threads=8;
		cpu=sc->x264_threads;
	}
	int format=X264_CSP_I420;
	if (yuv_format==1) format=X264_CSP_I444;
	if (yuv_format==2) format=X264_CSP_I420;

	x264_param_default_preset(&x264.param, "ultrafast","zerolatency");
	x264.param.i_csp=format;
	x264.param.i_width=sc->w;
	x264.param.i_height=sc->h;
	if (sc->x264_threads) x264.param.i_threads=cpu;
	else x264.param.i_threads=cpu/2;
	x264.param.i_fps_num=60;
	x264.param.i_fps_den=1;
	x264.param.rc.i_rc_method=X264_RC_CRF;
	x264.param.rc.f_rf_constant=18;//19;//22;//18;
	x264.param.rc.f_rf_constant_max=24;//26;//26;//24;
	x264.param.rc.i_lookahead=0;
	x264.param.b_repeat_headers=1;
	x264.param.b_annexb=1;
	x264.param.i_log_level=0;

	x264.encoder=x264_encoder_open(&x264.param);
	x264_picture_alloc(&x264.in,format,sc->w,sc->h);
}

int x264_encode()
{
	SCR *sc=&screen;

	struct timeval t1,t2;
	gettimeofday(&t1,NULL);

	int ret;
	ret=x264_encoder_encode(x264.encoder,&x264.nals,&x264.i_nals,&x264.in,&x264.out);

	gettimeofday(&t2,NULL);
	double td=((t2.tv_sec-t1.tv_sec)*1000000)+(t2.tv_usec-t1.tv_usec);
	sc->stat_x264_time+=td;

	return ret;
}

void x264_close()
{
	if (x264.encoder) x264_encoder_close(x264.encoder);
	x264_picture_clean(&x264.in);
}
