
#include <X11/extensions/Xdamage.h>

void damage()
{
	SCR *sc=&screen;

	Display *d=XOpenDisplay(NULL);
	Window w=DefaultRootWindow(d);

	Damage dmg=XDamageCreate(d,w,XDamageReportNonEmpty);
	sc->dmg_cnt=1;
	while (sc->end==0) {
		XEvent ev;
		XNextEvent(d,&ev);
		XDamageSubtract(d,dmg,None,None);
		sc->dmg_cnt++;
		sc->stat_dmg_cnt2++;
//		printf("dmg %i\n",sc->stat_dmg_cnt2);
	}
}
