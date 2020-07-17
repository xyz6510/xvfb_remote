#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>


#define keybuf_size 256*4
#define mousebuf_size 256*4

typedef struct {
	int e;
	int k;
	int m;
	int mm;
	int mle;
	int map;
	int min;
	int move;
	int res;
	int focus;
	int expose;
} XEVENT;

typedef struct {
	Display *d;
	Window root;
	Window w;
	Atom a;
	GLXContext g;
	int wx;
	int wy;
	int ww;
	int wh;
	int sww;
	int swh;
	int paint;
	pthread_t input_t;
	int *keybuf;
	int *mousebuf;
	XEVENT ev;
	int end;
} SCR;

SCR screen;
