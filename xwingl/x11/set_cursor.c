
void set_cursor(int c)
{
	Display *d=screen.d;
	Window w=screen.w;

	if (c) {
		XUndefineCursor(d,w);
	} else {
		char cd[1]={0};
		XColor xc;
		Pixmap nul=XCreateBitmapFromData(d,w,cd,1,1);
		Cursor cursor=XCreatePixmapCursor(d,nul,nul,&xc,&xc,0,0);
		XFreePixmap(d,nul);
		XDefineCursor(d,w,cursor);
	}
}
