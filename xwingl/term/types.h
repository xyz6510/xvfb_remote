#include <stdint.h>

#define terminal_size (32768)

typedef struct {
	uint8_t *buf;
	uint16_t *cbuf;
	uint16_t fgbg;
	int bpos;
	int x;
	int y;
	int w;
	int h;
	int cfgbg;
	int cidx;
	int cx;
	int cy;
	int tbs;
	int active;
} TERMINAL;

TERMINAL terminal;

