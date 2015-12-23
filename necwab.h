/*
 * Copyright (c) 2015 Kenji Aoyama.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

 /*
  *
  */

#define NECWAB_INDEX	0x0fa2
#define NECWAB_DATA	0x0fa3

extern int wab_fd;
extern u_int8_t *pc98iobase;
extern u_int8_t *pc98membase;
extern u_int8_t *wab_iobase;
extern u_int8_t *wab_membase;

struct board_type_t {
	int	type;
	int	offset;
};

int necwab_init(struct board_type_t *);
void necwab_fini(void);

/* internal use */
u_int8_t necwab_inb(u_int16_t);
int necwab_outb(u_int16_t, u_int8_t);
int necwab_outw(u_int16_t, u_int16_t);
int necwab_ident_board(struct board_type_t *);
