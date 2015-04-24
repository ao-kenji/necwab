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

#include "nec_gd542xreg.h"
#include "nec_cirrus.h"
#include "nec_s3.h"
#include "necwab.h"

int wab_iofd, wab_memfd;
u_int8_t *pc98iobase, *pc98membase;
u_int8_t *wab_iobase,*wab_membase;

int
necwab_init(void)
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

	board_type = necwab_ident_board();
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

int
necwab_write_reg(u_int16_t index, u_int8_t data)
{
	*(pc98iobase + index) = data;
}

inline int
necwab_outb(u_int16_t index, u_int8_t data)
{
	*(pc98iobase + index) = data;
}

int
necwab_outw(u_int16_t index, u_int16_t data)
{
#if 0
	necwab_outb(index + 0, (u_int8_t)(data >> 8));
	necwab_outb(index + 1, (u_int8_t)(data & 0xff));
#else
	necwab_outb(index + 0, (u_int8_t)(data & 0xff));
	necwab_outb(index + 1, (u_int8_t)(data >> 8));
#endif
}

u_int8_t
necwab_inb(u_int16_t index)
{
	return *(pc98iobase + index);
}

int
necwab_ident_board(void)
{
	u_int8_t data;

	necwab_outb(NECWAB_INDEX, 0x00);
	data = necwab_inb(NECWAB_DATA);

	switch (data) {
	case 0x20:
		printf("PC-9801-85(WAB-B) found\n");
		break;
	case 0x60:
		printf("PC-9801-96(WAB-B3) found\n");
		break;
	default:
		printf("No supported WAB found, ID = 0x%02x\n", data);
		data = -1;
		break;
	}

	return (int)data;
}

/* 256 bytes memory dump  */
void
necwab_dump(u_int8_t *addr)
{
	int i;

	for (i = 0; i < 256; i++) {
		if (i % 16 == 0)
			printf("0x%08x ", addr + i);
		printf("%02x ", *(addr + i));
		if (i % 16 == 15)
			printf("\n");
	}
}
