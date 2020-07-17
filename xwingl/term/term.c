
#include "types.h"

#include "terminal.c"

void init_terminal()
{
	TERMINAL *tr=&terminal;

	tr->buf=malloc(terminal_size);
	tr->cbuf=malloc(terminal_size*sizeof(uint16_t));

	term_reset();
}
