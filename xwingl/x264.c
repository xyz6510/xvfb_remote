
void x264()
{
	XWINGL_VAR *xwg=&xwingl_var;

	VIDEO_VAR *v=&video_var;

	const AVCodec *codec;
	AVCodecParserContext *parser;
	AVCodecContext *cx=NULL;
	AVFrame *frame;
	uint8_t *inbuf=malloc(8*1024*1024);
	int ret;
	AVPacket *pkt;
	char res[32];

l1:
	sprintf(res,"%ix%i",xwg->srv_w,xwg->srv_h);
	add_resolution(res);
	int i;
	for (i=0;i<xwg->resolutions.num;i++) {
		if ((xwg->resolutions.resx[i]==xwg->srv_w)&&(xwg->resolutions.resy[i]==xwg->srv_h)) {
			xwg->res_idx=i;
			break;
		}
	}

	pkt=av_packet_alloc();
	codec=avcodec_find_decoder(AV_CODEC_ID_H264);
	parser=av_parser_init(codec->id);
	cx=avcodec_alloc_context3(codec);
	if((codec->capabilities)&AV_CODEC_CAP_TRUNCATED) cx->flags|=AV_CODEC_FLAG_TRUNCATED;
	cx->thread_count=sysconf(_SC_NPROCESSORS_ONLN);
	cx->thread_type=FF_THREAD_SLICE;// for low latency else 5 frame delay !!!!
	avcodec_open2(cx,codec,NULL);
	frame=av_frame_alloc();
	int type;
	int size;
	int e;
	while(xwg->end==0) {
		e=read_data(xwg->conn_x264,&type,4,NULL);if (e<=0) break;
		//printf("type:%i\n",type);
		if (type==60) {
			e=read_data(xwg->conn_x264,&xwg->srv_w,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_x264,&xwg->srv_h,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_x264,&xwg->srv_format,4,NULL);if (e<=0) break;
			e=read_data(xwg->conn_x264,&xwg->srv_max_fps,4,NULL);if (e<=0) break;

			av_parser_close(parser);
			avcodec_free_context(&cx);
			av_frame_free(&frame);
			av_packet_free(&pkt);
			xwg->reconfigure_screen=1;
			goto l1;
		}
		if (type==55) {
			e=read_data(xwg->conn_x264,&size,4,NULL);if (e<=0) break;
			int rd=0;
			e=read_data(xwg->conn_x264,inbuf,size,&rd);if (e<=0) break;
			ret=av_parser_parse2(parser,cx,&pkt->data,&pkt->size,
				inbuf,rd,AV_NOPTS_VALUE, AV_NOPTS_VALUE,0);
			if (ret==rd) {
				ret=av_parser_parse2(parser,cx,&pkt->data,&pkt->size,
					inbuf,rd,AV_NOPTS_VALUE,AV_NOPTS_VALUE,0);
			}
			if (pkt->size) {
				ret=avcodec_send_packet(cx,pkt);
    			if ((ret<0)&&(ret!=AVERROR(EAGAIN))&&(ret!=AVERROR_EOF)) {
    				printf("error x264:%i\n",ret);
	    			exit(-1);
				} else {
					pkt->size=0;
					ret=avcodec_receive_frame(cx,frame);
					if ((ret>=0)&&(frame->data[0])) {
						v->y_line_size=frame->linesize[0];
						v->u_line_size=frame->linesize[1];
						v->v_line_size=frame->linesize[2];
						int uvy=v->res_y/xwg->srv_format;
						memcpy(v->y_data,frame->data[0],frame->linesize[0]*v->res_y);
						memcpy(v->u_data,frame->data[1],frame->linesize[1]*uvy);
						memcpy(v->v_data,frame->data[2],frame->linesize[2]*uvy);
						v->yuv_update++;
					}
				}
			}
		}
	}
	close_socket(xwg->conn_x264);
	av_parser_close(parser);
	avcodec_free_context(&cx);
	av_frame_free(&frame);
	av_packet_free(&pkt);
	xwg->end=1;
	screen.end=1;

	printf("connection x264 close\n");
}
