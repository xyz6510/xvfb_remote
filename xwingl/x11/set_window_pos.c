
void set_window_pos(int px,int py,int resizable)//px=-1 center x ,py=-1 center y
{
	int wr,hr,wx,hy;

	get_root_size(&wr,&hr);
	wx=screen.ww;
	hy=screen.wh;
	if (resizable==0) {
		wx=screen.sww;
		hy=screen.swh;
	}
	if (px==-1) px=(wr-wx)/2;
	if (py==-1) py=(hr-hy)/2;
	XMoveWindow(screen.d,screen.w,px,py);
}
