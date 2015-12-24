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

#include <stdio.h>
#include <fcntl.h>		/* open(2) */
#include <unistd.h>		/* usleep(3) */
#include <sys/ioctl.h>		/* ioctl(2) */
#include <sys/mman.h>		/* mmap(2) */
#include <sys/types.h>

#include "necwab.h"
#include "nec_cirrus.h"
#include "nec_s3.h"
#include "color.h"

int wab_iofd, wab_memfd;
u_int8_t *pc98iobase, *pc98membase;
u_int8_t *wab_iobase,*wab_membase;

int
necwab_init(struct board_type_t *bt)
{
	int board_type;

	wab_iofd = open("/dev/pcexio", O_RDWR, 0600);
	if (wab_iofd == -1) {
		perror("open");
		goto exit1;
	}

	pc98iobase = (u_int8_t *)mmap(NULL, 0x10000, PROT_READ | PROT_WRITE,
	    MAP_SHARED, wab_iofd, 0);
	if (pc98iobase == MAP_FAILED) {
		perror("mmap iobase");
		goto exit2;
	}
	wab_iobase = pc98iobase + 0x0000;

	board_type = necwab_ident_board(bt);
	if (board_type == -1) {
		printf("No WAB found\n");
		goto exit2;
	}

	wab_memfd = open("/dev/pcexmem", O_RDWR, 0600);
	if (wab_memfd == -1) {
		perror("open mem");
		goto exit2;
	}

	pc98membase = (u_int8_t *)mmap(NULL, 0x1000000, PROT_READ | PROT_WRITE,
	    MAP_SHARED, wab_memfd, 0);
	if (pc98membase == MAP_FAILED) {
		perror("mmap membase");
		goto exit3;
	}
	wab_membase = pc98membase + 0x0000;
	return board_type;

exit3:
	close(wab_memfd);
exit2:
	close(wab_iofd);
exit1:
	return -1;
}
	
void
necwab_fini(void)
{
	munmap((void *)pc98membase, 0x1000000);
	close(wab_memfd);
	munmap((void *)pc98iobase, 0x10000);
	close(wab_iofd);
}

u_int8_t
necwab_inb(u_int16_t index)
{
	return *(pc98iobase + index);
}

inline int
necwab_outb(u_int16_t index, u_int8_t data)
{
	*(pc98iobase + index) = data;
}

int
necwab_outw(u_int16_t index, u_int16_t data)
{
	/* luna88k is big endian */
	necwab_outb(index + 0, (u_int8_t)(data & 0xff));
	necwab_outb(index + 1, (u_int8_t)(data >> 8));
}

int
necwab_ident_board(struct board_type_t *bt)
{
	u_int8_t data;
	u_int i;

	bt->offset = 0;

	/* first, try to check 3rd-party-board */
	for (i = 0; i < 0x0f; i += 2) {
		data = necwab_inb(0x51e1 + i);
		if (data == 0xc2) {
			printf("MELCO WGN-A/WSN-A found, offset 0x%02x\n", i);
			bt->type = data;
			bt->offset = i;
#if 0
			/* XXX: special initialize? */
			necwab_outb(0x42e1 + i, 0x08);
			data = necwab_inb(0x57e1 + i);
			necwab_outb(0x57e1 + i, data & 0x7f);
#endif
			return (int)data;
		}
	}

	necwab_outb(NECWAB_INDEX, 0x00);
	data = necwab_inb(NECWAB_DATA);

	switch (data) {
	case 0x20:
		printf("PC-9801-85(WAB-B) found\n");
		bt->type = data;
		break;
	case 0x60:
		printf("PC-9801-96(WAB-B3) found\n");
		bt->type = data;
		break;
	default:
		printf("No supported WAB found, ID = 0x%02x\n", data);
		data = -1;
		bt->type = data;
		break;
	}

	return (int)data;
}

