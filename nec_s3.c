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
/* #include "s3reg.h" */
#include "nec_s3.h"

extern int wab_iofd, wab_memfd;
extern u_int8_t *pc98iobase, *pc98membase;
extern u_int8_t *wab_iobase,*wab_membase;

u_int16_t _port_tbl[0x3e0];

void
nec_s3_main(void)
{
	u_int8_t data;

	int i;
	for (i = 0; i < 16; i++) {
		necwab_outb(NECWAB_INDEX, (u_int8_t)i);
		data = necwab_inb(NECWAB_DATA);
		printf("0x%02x -> 0x%02x\n", i, data);
	}

	/* memory mapped I/O = 0x000d0000 */
	necwab_outb(NECWAB_INDEX, 0x01);
	necwab_outb(NECWAB_DATA, 0x5f);	/* 0xNN0df000-0xNN0dffff */
	necwab_outb(NECWAB_INDEX, 0x02);
	necwab_outb(NECWAB_DATA, 0x00);	/* specify above NN */

	u_int8_t *base = pc98membase;
	int offset = 0x000df000 + 0x200;

	nec_s3_reg_on();
	nec_s3_disp_on();

	for (i = 0; i < 256; i++) {
		printf("%02x ", *(base + offset + i));
		if (i % 16 == 15)
			printf("\n");
	}
	printf("\n");

	necwab_outb(0x240, 0x1e);	/* ROM_PAGE_SEL 0x1e */
	necwab_outb(0x002, 0x01);	/* 0x102, 0x01 */
	necwab_outb(0x240, 0x0e);	/* ROM_PAGE_SEL, 0x0e */
	necwab_outb(0x248, 0x05);	/* ADVFUNC_CNTL, 0x0005 */
	necwab_outb(0x1c2, 0xcd);	/* 0x3c2, 0xcd */
	necwab_outb(0x1da, 0x00);	/* 0x3da, 0x00 */

	for (i = 0; i < 256; i++) {
		printf("%02x ", *(base + offset + i));
		if (i % 16 == 15)
			printf("\n");
	}

	nec_s3_disp_off();
	nec_s3_reg_off();
}

void
nec_s3_reg_on(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data |= 0x01;
	necwab_outb(NECWAB_DATA, data);
}

void
nec_s3_reg_off(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data &= ~0x01;
	necwab_outb(NECWAB_DATA, data);
}

void
nec_s3_disp_on(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data |= 0x02;
	data = necwab_inb(NECWAB_DATA);
	necwab_outb(NECWAB_DATA, data);
}

void
nec_s3_disp_off(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data &= ~0x02;
	necwab_outb(NECWAB_DATA, data);
}

/*
 * taken from (ancient)
 * XFree86/xc/programs/Xserver/hw/xfree98/accel/s3nec/s3pc98.c
 */

void _necportconvert()
{
	int i;
	for(i=0x3b0; i<0x3e0; i++){
		_port_tbl[i] = i - 0x200;
	}
	_port_tbl[0x102] = 0x002;
	_port_tbl[0x42] =  0x240;
	_port_tbl[0x46] =  0x244;
	_port_tbl[0x4a] =  0x248;
	_port_tbl[0x82] =  0x280;
	_port_tbl[0x86] =  0x284;
	_port_tbl[0x8a] =  0x288;
	_port_tbl[0x8e] =  0x28c;
	_port_tbl[0x92] =  0x290;
	_port_tbl[0x96] =  0x294;
	_port_tbl[0x9a] =  0x298;
	_port_tbl[0x9e] =  0x29c;
	_port_tbl[0xa2] =  0x2a0;
	_port_tbl[0xa6] =  0x2a4;
	_port_tbl[0xaa] =  0x2a8;
	_port_tbl[0xae] =  0x2ac;
	_port_tbl[0xb2] =  0x2b0;
	_port_tbl[0xb6] =  0x2b4;
	_port_tbl[0xba] =  0x2b8;
	_port_tbl[0xbe] =  0x2bc;
	_port_tbl[0xe2] =  0x2e0;
}
