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
#include <sys/types.h>

#include "nec_gd542xreg.h"
#include "nec_cirrus.h"
#include "necwab.h"
#include "necwabvar.h"

void	nec_cirrus_chip_init(void);
void	nec_cirrus_enter(void);
void	nec_cirrus_leave(void);

int
nec_cirrus_main(void)
{
	u_int8_t data;
	u_int8_t *addr;
	int i, j;

	/* set VRAM window address to 0xf00000-0xf0ffff */
	necwab_outb(NECWAB_INDEX, 0x01);
	necwab_outb(NECWAB_DATA, 0xa0);

	/* enable WAB chip register */
	nec_cirrus_enter();
	nec_cirrus_chip_init();

	printf("Chip ID: 0x%02x\n", nec_cirrus_chip_id());

	for (i = 0; i < 640*480; i++) {
		nec_cirrus_write(i, 0);
	}

	for (i = 0; i < 480; i++) {
		nec_cirrus_write(640 * i + i, i % 256);
	}
#if 0
	for (i = 0; i < 640; i++) {
		nec_cirrus_write(640 * 100 + i, 100);
	}
#endif

	necwab_dump(0);

	getchar();

	nec_cirrus_leave();

	return 0;
}

int
nec_cirrus_chip_id(void)
{
	u_int8_t data;

	necwab_outb(GD542X_REG_3D4, 0x27);
	data = necwab_inb(GD542X_REG_3D5);
	data >>= 2;	/* bit 7-2 has Chip ID */
	return data;
}

/* enable/disable GD5428 registers on WAB */
void
nec_cirrus_reg_on(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data |= 0x01;
	necwab_outb(NECWAB_DATA, data);
}

void
nec_cirrus_reg_off(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data &= ~0x01;
	necwab_outb(NECWAB_DATA, data);
}

/* on/off display relay on WAB */
void
nec_cirrus_disp_on(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data |= 0x02;
	necwab_outb(NECWAB_DATA, data);
}

void
nec_cirrus_disp_off(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data &= ~0x02;
	necwab_outb(NECWAB_DATA, data);
}

void
nec_cirrus_write(u_int32_t addr, u_int8_t value)
{
	u_int32_t offset;
	u_int8_t  base;
	u_int8_t *waddr;

	base   = (u_int8_t)((addr & 0x000ff000) >> 12);
	offset = (u_int32_t)(addr & 0x00000fff);
#if 0
	printf("0x%02x, 0x%04x, 0x%02x\n", base, offset, value);
#endif

	necwab_outb(GD542X_REG_3CE, 0x09);
	necwab_outb(GD542X_REG_3CF, base);

	waddr = (u_int8_t *)(pc98membase + 0x00f00000 + offset);
	*waddr = value;
}

u_int8_t
nec_cirrus_read(u_int32_t addr)
{
	u_int32_t offset;
	u_int8_t  base;
	u_int8_t *waddr;

	base   = (u_int8_t)((addr & 0x000f0000) >> 12);
	offset = (u_int32_t)(addr & 0x0000ffff);
#if 0
	printf("0x%02x, 0x%04x, 0x%02x\n", base, offset, value);
#endif

	necwab_outb(GD542X_REG_3CE, 0x09);
	necwab_outb(GD542X_REG_3CF, base);

	waddr = (u_int8_t *)(pc98membase + 0x00f00000 + offset);
	return *waddr;
}

/* unlock/lock Cirrus extension */
void
nec_cirrus_unlock(void)
{
	/* 0x12 -> SR6 */ 
	necwab_outw(GD542X_REG_3C4, 0x0612);
}

void
nec_cirrus_lock(void)
{
	/* 0x00 -> SR6 */ 
	necwab_outw(GD542X_REG_3C4, 0x0600);
}

void
nec_cirrus_enter(void)
{
	/* enable WAB registers & video output */
	necwab_outb(NECWAB_INDEX, 0x03);
	necwab_outb(NECWAB_DATA, 0x03);

	necwab_outb(GD542X_REG_094, 0x01);
	necwab_outb(GD542X_REG_102, 0x01);
	necwab_outb(GD542X_REG_3C2, 0x01);
	necwab_outb(GD542X_REG_094, 0x20);
	necwab_outb(GD542X_REG_3C2, 0xe1);
	necwab_outb(GD542X_REG_3DA, 0x00);
}

void
nec_cirrus_leave(void)
{
	necwab_outb(GD542X_REG_3C0, 0x1f);
	necwab_inb(GD542X_REG_3C6);	/* dummy read 4 times to access */
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_outb(GD542X_REG_3C6, 0x80);

	/* disable WAB registers & video output */
	necwab_outb(NECWAB_INDEX, 0x03);
	necwab_outb(NECWAB_DATA, 0x00);
}

void
nec_cirrus_chip_init(void)
{
	int i;
	u_int8_t *c;

	nec_cirrus_unlock();

	/* MISC */
	necwab_outb(GD542X_REG_3C2, 0xef);
	/* enable VGA subsystem */
	necwab_outb(GD542X_REG_3C3, 0x01);

	/* SR: Sequence Control Register */
	for (c = nec_cirrus_SRdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3C4, *c++);
		necwab_outb(GD542X_REG_3C5, *c++);
	}

#ifdef USE_16BIT
	/* color mode */
	necwab_outb(GD542X_REG_3C4, 0x0f);
	necwab_outb(GD542X_REG_3C5, 0x95);
	necwab_outb(GD542X_REG_3C4, 0x07);
	necwab_outb(GD542X_REG_3C5, 0x07);
#endif
	/*
	 * 640x480 (60Hz): 0x65, 0x3b
	 * 1024x768 : 0x4d, 0x22 
	 */
	necwab_outb(GD542X_REG_3C4, 0x0e);
	necwab_outb(GD542X_REG_3C5, 0x65);
	necwab_outb(GD542X_REG_3C4, 0x1e);
	necwab_outb(GD542X_REG_3C5, 0x3b);

	/* CR: */
	/* unlock CR0-7 */
	necwab_outb(GD542X_REG_3D4, 0x11);
	necwab_outb(GD542X_REG_3D5, 0x00);	/* 0x20? */

	for (c = nec_cirrus_CRdata_640; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3D4, *c++);
		necwab_outb(GD542X_REG_3D5, *c++);
	}

	/* GR */
	for (c = nec_cirrus_GRdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3CE, *c++);
		necwab_outb(GD542X_REG_3CF, *c++);
	}

	/* AR */
	/* clear internal flip-flop status for AR */
	necwab_inb(GD542X_REG_3DA);
	for (c = nec_cirrus_ARdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3C0, *c++);
		necwab_outb(GD542X_REG_3C0, *c++);
	}
	necwab_inb(GD542X_REG_3DA);
	/* ARX <- 0x20: Video Enable */
	necwab_outb(GD542X_REG_3C0, 0x20);

	/* pixel mask register <- 0xff: color mode through */
	necwab_outb(GD542X_REG_3C6, 0xff);

	/* HDR: Hidden DAC Register */
	necwab_inb(GD542X_REG_3DA);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
#ifdef USE_16BIT
	/* 0xc1 = 16bit, 5-6-5 XGA */
	necwab_outb(GD542X_REG_3C6, 0xc1);
#else
	/* 0x00 = VGA compat., 0xc9 = 6bit, */
	necwab_outb(GD542X_REG_3C6, 0xc9);
#endif

	/* test palette */
	necwab_outb(GD542X_REG_3C8, 0);	/* reset */
	for(i = 0; i < sizeof(default_pal) / sizeof(default_pal[0]); i++) {
		/* data (R,G,B) */
		necwab_outb(GD542X_REG_3C9, default_pal[i].r);
		necwab_outb(GD542X_REG_3C9, default_pal[i].g);
		necwab_outb(GD542X_REG_3C9, default_pal[i].b);
	}
}
