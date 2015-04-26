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
void	nec_cirrus_chip_init2(void);
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
	nec_cirrus_chip_init2();

	printf("Chip ID: 0x%02x\n", nec_cirrus_chip_id());

	for (i = 0; i < 640*480; i++) {
#if 0
		nec_cirrus_write(i, i % 256);
#else
		nec_cirrus_write(i, 0);
#endif
	}

#if 0
	i = 240 * 640 + 320;
	nec_cirrus_write(i, i % 256);

	necwab_outb(GD542X_REG_3CE, 0x09);
	necwab_outb(GD542X_REG_3CF, (i & 0xf0000)>>12);
#endif

	i = 0;
	addr = (u_int8_t *)(pc98membase + 0x00f00000 + (i & 0xffff));
	necwab_dump(addr);

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

	base   = (u_int8_t)((addr & 0x000f0000) >> 12);
	offset = (u_int32_t)(addr & 0x0000ffff);
#if 0
	printf("0x%02x, 0x%04x, 0x%02x\n", base, offset, value);
#endif

	necwab_outb(GD542X_REG_3CE, 0x09);
	necwab_outb(GD542X_REG_3CF, base);

	waddr = (u_int8_t *)(pc98membase + 0x00f00000 + offset);
	*waddr = value;
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

/* values to intialize cirrus GD54xx specific ext registers */
/* XXX these values are taken from PC XXX */
static const u_int8_t vga_gd54xx[] = {
	0x0f,	/* 05: ??? */
	0x12,	/* 06: enable ext reg (?) */
	0x01,	/* 07: High-Resolution 256-Color */
	0x00,	/* 08: EEPROM control register */
	0x5c,	/* 09: BIOS Scratch register for 542x (?) */
	0x09,	/* 0A: BIOS Scratch register for 542x (?) */
	0x4a,	/* 0B: VCLK0 frequency numerator */
	0x5b,	/* 0C: VCLK1 frequency numerator */
	0x42,	/* 0D: VCLK2 frequency numerator */
	0x00,	/* 0E: VCLK3 frequency numerator */
	0x10,	/* 0F: DRAM control register */
	0x00,	/* 10: Graphic cursor X position */
	0x00,	/* 11: Graphic cursor Y position */
	0x00,	/* 12: Graphic cursor attribute */
	0x00,	/* 13: Graphic cursor pattern address */
	0x00,	/* 14: BIOS scratch register for 546x (?) */
	0x00,	/* 15: BIOS scratch register for 546x (?) */
	0xf8,	/* 16: Performance Tuning */
	0x39,	/* 17: ??? */
	0x00,	/* 18: ??? */
	0x01,	/* 19: ??? */
	0x00,	/* 1A: ??? */
	0x2b,	/* 1B: VCLK0 denominator and post-scalar value */
	0x2f,	/* 1C: VCLK1 denominator and post-scalar value */
	0x1f,	/* 1D: VCLK2 denominator and post-scalar value */
	0x00,	/* 1E: VCLK3 denominator and post-scalar value */
	0x19	/* 1F: MCLK (?) */
};

void
nec_cirrus_chip_init(void)
{
	int i;

	nec_cirrus_unlock();

	/* XX:400 line, upper page, reserved, 25.175Mhz, RAM enable, color */
	necwab_outb(GD542X_REG_3C2, 0xef);
	/* enable VGA subsystem */
	necwab_outb(GD542X_REG_3C3, 0x01);

	/* SR: Sequence Control Register */
	necwab_outb(GD542X_REG_3C4, 0x00);
	necwab_outb(GD542X_REG_3C5, 0x01);

	for(i = 1; i < sizeof(modeXX_SR) / sizeof(modeXX_SR[0]); i++) {
		necwab_outb(GD542X_REG_3C4, (u_int8_t)i);
		necwab_outb(GD542X_REG_3C5, modeXX_SR[i]);
	}
	/* cirrus extended */
	for(i = 0; i < sizeof(vga_gd54xx) / sizeof(vga_gd54xx[0]); i++) {
		necwab_outb(GD542X_REG_3C4, (u_int8_t)(i + 5));
		necwab_outb(GD542X_REG_3C5, vga_gd54xx[i]);
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

	for(i = 0; i < sizeof(modeXX_CR) / sizeof(modeXX_CR[0]); i++) {
		necwab_outb(GD542X_REG_3D4, (u_int8_t)i);
		necwab_outb(GD542X_REG_3D5, modeXX_CR[i]);
	}

	/* GR */
	for(i = 0; i < sizeof(modeXX_GR) / sizeof(modeXX_GR[0]); i++) {
		necwab_outb(GD542X_REG_3CE, (u_int8_t)i);
		necwab_outb(GD542X_REG_3CF, modeXX_GR[i]);
	}

	/*
	 * GRB <- 0x0e: disable offset reg#1, enable BY8 addressing,
	 *              enable extended write modes,
	 *		and enable 8-byte data latches
	 * see TRM D6
	 */
	necwab_outb(GD542X_REG_3CE, 0x0b);
	necwab_outb(GD542X_REG_3CF, 0x0e);

	/* a0000 - affff 64KB, normal, graphic mode */
	necwab_outb(GD542X_REG_3CE, 0x06);
	necwab_outb(GD542X_REG_3CF, 0x05);

	/* AR */
	/* clear internal flip-flop status for AR */
	necwab_inb(GD542X_REG_3DA);
	for(i = 0; i < sizeof(modeXX_AR) / sizeof(modeXX_AR[0]); i++) {
		necwab_outb(GD542X_REG_3C0, (u_int8_t)i);
		necwab_outb(GD542X_REG_3C0, modeXX_AR[i]);
	}
	necwab_inb(GD542X_REG_3DA);
	/* ARX <- 0x20: Video Enable */
	necwab_outb(GD542X_REG_3C0, 0x20);

	/* pixel mask register <- 0xff: color mode through */
	necwab_outb(GD542X_REG_3C6, 0xff);

	/* HDR: Hidden DAC register ? */
	necwab_inb(GD542X_REG_3DA);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
#ifdef USE_16BIT
	/* 0xe1 = 16bit, 5-6-5 */
	necwab_outb(GD542X_REG_3C6, 0x01);
#else
	/* 0x70 = VGA compat., 0xf0 = 6bit, */
	necwab_outb(GD542X_REG_3C6, 0x70);
#endif

	/* palette */
	necwab_outb(GD542X_REG_3C8, 0);	/* reset */
	for(i = 0; i < sizeof(default_pal) / sizeof(default_pal[0]); i++) {
		/* data (R,G,B) */
		necwab_outb(GD542X_REG_3C9, default_pal[i].r);
		necwab_outb(GD542X_REG_3C9, default_pal[i].g);
		necwab_outb(GD542X_REG_3C9, default_pal[i].b);
	}

	necwab_outb(GD542X_REG_3C7, 0);	/* reset */
	for(i = 0; i < sizeof(default_pal) / sizeof(default_pal[0]); i++) {
		int r, g, b;
		/* data (R,G,B) */
		r = necwab_inb(GD542X_REG_3C9);
		g = necwab_inb(GD542X_REG_3C9);
		b = necwab_inb(GD542X_REG_3C9);
		printf("pal %03d, (%02x, %02x, %02x)\n",
			i, r, g, b);
	}
}

void
nec_cirrus_chip_init2(void)
{
	int i;
	u_int8_t *c;

	nec_cirrus_unlock();

	/* MISC */
	necwab_outb(GD542X_REG_3C2, 0xef);
	/* enable VGA subsystem */
	necwab_outb(GD542X_REG_3C3, 0x01);

	/* SR: Sequence Control Register */
	for (c = &nec_cirrus_SRdata; *c != 0xff; /* empty */) {
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

	for (c = &nec_cirrus_CRdata_640; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3D4, *c++);
		necwab_outb(GD542X_REG_3D5, *c++);
	}

	/* GR */
	for (c = &nec_cirrus_GRdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3CE, *c++);
		necwab_outb(GD542X_REG_3CF, *c++);
	}

	/* AR */
	/* clear internal flip-flop status for AR */
	necwab_inb(GD542X_REG_3DA);
	for (c = &nec_cirrus_ARdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3C0, *c++);
		necwab_outb(GD542X_REG_3C0, *c++);
	}
	necwab_inb(GD542X_REG_3DA);
	/* ARX <- 0x20: Video Enable */
	necwab_outb(GD542X_REG_3C0, 0x20);

	/* pixel mask register <- 0xff: color mode through */
	necwab_outb(GD542X_REG_3C6, 0xff);

	/* HDR: Hidden DAC register ? */
	necwab_inb(GD542X_REG_3DA);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
#ifdef USE_16BIT
	/* 0xe1 = 16bit, 5-6-5 */
	necwab_outb(GD542X_REG_3C6, 0x01);
#else
	/* 0x70 = VGA compat., 0xf0 = 6bit, */
	necwab_outb(GD542X_REG_3C6, 0xf0);
#endif

	/* palette */
	necwab_outb(GD542X_REG_3C8, 0);	/* reset */
	for(i = 0; i < sizeof(default_pal) / sizeof(default_pal[0]); i++) {
		/* data (R,G,B) */
		necwab_outb(GD542X_REG_3C9, default_pal[i].r);
		necwab_outb(GD542X_REG_3C9, default_pal[i].g);
		necwab_outb(GD542X_REG_3C9, default_pal[i].b);
	}

	necwab_outb(GD542X_REG_3C7, 0);	/* reset */
	for(i = 0; i < sizeof(default_pal) / sizeof(default_pal[0]); i++) {
		int r, g, b;
		/* data (R,G,B) */
		r = necwab_inb(GD542X_REG_3C9);
		g = necwab_inb(GD542X_REG_3C9);
		b = necwab_inb(GD542X_REG_3C9);
		printf("pal %03d, (%02x, %02x, %02x)\n",
			i, r, g, b);
	}
}
