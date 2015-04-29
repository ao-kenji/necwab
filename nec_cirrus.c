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

#if 0
#define USE_8BIT_RGB
#endif

#include <stdio.h>
#include <sys/types.h>

#include "nec_gd542xreg.h"
#include "nec_cirrus.h"
#include "necwab.h"
#include "color.h"

void	nec_cirrus_chip_init(void);
void	nec_cirrus_dump(u_int32_t);
void	nec_cirrus_enter(void);
void	nec_cirrus_leave(void);
void	nec_cirrus_set_base(u_int8_t);
void	nec_cirrus_set_default_cmap(void);

void	draw_box(int, int, int, int, int);

int
nec_cirrus_main(void)
{
	u_int8_t data;
	u_int8_t *addr;
	int i, j;

	/* set VRAM window address to 0xf00000-0xf0ffff */
	necwab_outb(NECWAB_INDEX, 0x01);
	necwab_outb(NECWAB_DATA, 0xa0);
	wab_membase = pc98membase + 0x00f00000;

	/* enable WAB chip register */
	nec_cirrus_enter();

	printf("Chip ID = 0x%02x\n", nec_cirrus_chip_id());

	nec_cirrus_chip_init();

	/* clear */
	for (i = 0; i < 1024*768; i++) {
		nec_cirrus_write(i, 0);
	}

	/* write 256 color matrix */
#define XSIZE 48
#define YSIZE 48
	for (j = 0; j < 16; j++)
		for (i = 0; i < 16; i++)
			draw_box(i * XSIZE, j * YSIZE,
				 (i + 1) * XSIZE - 2, (j + 1) * YSIZE - 2,
				j * 16 + i);

	nec_cirrus_dump(0);

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

/*
 * enable GD5428 registers on WAB
 */
void
nec_cirrus_reg_on(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data |= 0x01;
	necwab_outb(NECWAB_DATA, data);
}

/*
 * disable GD5428 registers on WAB
 */
void
nec_cirrus_reg_off(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data &= ~0x01;
	necwab_outb(NECWAB_DATA, data);
}

/*
 * turn on display relay on WAB
 */
void
nec_cirrus_disp_on(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data |= 0x02;
	necwab_outb(NECWAB_DATA, data);
}

/*
 * turn off display relay on WAB
 */
void
nec_cirrus_disp_off(void)
{
	u_int8_t data;
	necwab_outb(NECWAB_INDEX, 0x03);
	data = necwab_inb(NECWAB_DATA);
	data &= ~0x02;
	necwab_outb(NECWAB_DATA, data);
}

/*
 * set base register (with simple cache)
 */
void
nec_cirrus_set_base(u_int8_t base) {
	static u_int8_t base0;	/* cache */

	if (base0 != base) {
		necwab_outb(GD542X_REG_3CE, 0x09);
		necwab_outb(GD542X_REG_3CF, base);
		base0 = base;
	}
}

/*
 * write 8bit data to vram address
 */
void
nec_cirrus_write(u_int32_t addr, u_int8_t value)
{
	u_int32_t offset;
	u_int8_t  base;
	u_int8_t *waddr;

	base   = (u_int8_t)((addr & 0x000fc000) >> 12);
	offset = (u_int32_t)(addr & 0x00003fff);	/* 16K */
#if 0
	printf("0x%02x, 0x%04x, 0x%02x\n", base, offset, value);
#endif

	nec_cirrus_set_base(base);

	waddr = (u_int8_t *)(wab_membase + offset);
	*waddr = value;
}

/*
 * read 8bit data from vram address
 */
u_int8_t
nec_cirrus_read(u_int32_t addr)
{
	u_int32_t offset;
	u_int8_t  base;
	u_int8_t *raddr;

	base   = (u_int8_t)((addr & 0x000fc000) >> 12);
	offset = (u_int32_t)(addr & 0x00003fff);	/* 16K */
#if 0
	printf("0x%02x, 0x%04x\n", base, offset);
#endif
	nec_cirrus_set_base(base);

	raddr = (u_int8_t *)(wab_membase + offset);
	return *raddr;
}

/*
 * 256 bytes vram memory dump
 */
void
nec_cirrus_dump(u_int32_t vram_addr)
{
	int i;

	for (i = 0; i < 256; i++) {
		if (i % 16 == 0)
			printf("0x%08x ", vram_addr + i);
		printf("%02x ", nec_cirrus_read(vram_addr + i));
		if (i % 16 == 15)
			printf("\n");
	}
}

/*
 * unlock Cirrus extension
 */
void
nec_cirrus_unlock(void)
{
	/* 0x12 -> SR6 */ 
	necwab_outw(GD542X_REG_3C4, 0x0612);
}

/*
 * lock Cirrus extension
 */
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

static u_int8_t nec_cirrus_SRdata[] = {
	0x00,0x02,0x00,0x03,0x01,0x01,0x02,0xFF,
	0x03,0x00,0x04,0x0E,0x05,0x0F,0x06,0x12,	/* SR4 0x0e -> 0x06? */
	0x07,0x01,0x08,0x00,0x09,0x6C,0x0A,0x59,	/* SR7 0x07 -> 0x01 if 8 bit? */
	0x0B,0x4A,0x0C,0x5B,0x0D,0x42,0x0E,0x54,
	0x0F,0xBD,0x10,0x00,0x11,0x00,0x12,0x00,
	0x13,0x3F,0x14,0x00,0x15,0x00,0x16,0x21,
	0x17,0x39,0x18,0x02,0x19,0x01,0x1A,0x00,
	0x1B,0x2B,0x1C,0x2F,0x1D,0x1F,0x1E,0x3B,
	0x1F,0x1F,0xFF
};

static u_int8_t nec_cirrus_CRdata_640[] = {
	0x00,0x5E,0x01,0x4F,0x02,0x50,0x03,0x82,
	0x04,0x54,0x05,0x9F,0x06,0x0B,0x07,0x3E,
	0x08,0x00,0x09,0x40,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0xEA,0x11,0x8C,0x12,0xDF,0x13,0xA0,
	0x14,0x00,0x15,0xE7,0x16,0x04,0x17,0xE3,
	0x18,0xFF,0x19,0x00,0x1A,0x00,0x1B,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_800[] = {
	0x00,0x7F,0x01,0x63,0x02,0x64,0x03,0x82,
	0x04,0x6A,0x05,0x1A,0x06,0x72,0x07,0xF0,
	0x08,0x00,0x09,0x60,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0x58,0x11,0x8C,0x12,0x57,0x13,0xC8,
	0x14,0x00,0x15,0x58,0x16,0x72,0x17,0xE3,
	0x18,0xFF,0x19,0x00,0x1A,0x00,0x1B,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_1024[] = {
	0x00,0xA1,0x01,0x7F,0x02,0x80,0x03,0x85,
	0x04,0x85,0x05,0x96,0x06,0x24,0x07,0xFD,
	0x08,0x00,0x09,0x60,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x08,0x0F,0x20,
	0x10,0x02,0x11,0x00,0x12,0xFF,0x13,0x80,
	0x14,0x00,0x15,0x00,0x16,0x26,0x17,0xE3,
	0x18,0xFF,0x19,0x32,0x1A,0xE0,0x1B,0x22,
#if 0
	0x1C,0x23,0x1D,0x22,0x1E,0x21,0x1F,0x20,
	0x20,0x1F,0x21,0x1E,0x22,0xF7,0x23,0x00,
	0x24,0x00,0x25,0x4a,0x26,0x20,
#endif
	0xFF
};

static u_int8_t nec_cirrus_GRdata[] = {
	0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,
	0x04,0x00,0x05,0x00,0x06,0x05,0x07,0x0F,
	0x08,0xFF,0x09,0x7F,0x0A,0x80,0x0B,0x0C,	/* GRB 0x0E -> 0x0C?*/
	0x0C,0xFF,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0xFF,0x11,0x00,0x12,0xFF,0x13,0xFF,
	0x14,0x00,0x15,0x00,0xFF
};

static u_int8_t nec_cirrus_ARdata[] = {
	0x10,0x41,0x11,0x00,0x12,0x0F,0x13,0x00,
	0x14,0x00,0xFF
};

struct {
	u_int8_t numer;
	u_int8_t denom;
} VCLK[] = {
	{ 0x65, 0x3b },	/* 640x480, V 60Hz, H 32kHz / 0x3a is NG */
	{ 0x5f, 0x23 },	/* 800x600, V 60Hz, H 38kHz / 0x22 is NG */
	{ 0x4d, 0x22 }	/* 1024x768, V 60Hz, H 49kHz */
};

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
	 * 800x600       : 0x5f, 0x23
	 * 1024x768      : 0x4d, 0x22 
	 */
	necwab_outb(GD542X_REG_3C4, 0x0e);
	necwab_outb(GD542X_REG_3C5, VCLK[2].numer);
	necwab_outb(GD542X_REG_3C4, 0x1e);
	necwab_outb(GD542X_REG_3C5, VCLK[2].denom);

	/* CR: */
	/* unlock CR0-7 */
	necwab_outb(GD542X_REG_3D4, 0x11);
	necwab_outb(GD542X_REG_3D5, 0x00);	/* 0x20? */

	for (c = nec_cirrus_CRdata_1024; *c != 0xff; /* empty */) {
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
#if defined(USE_16BIT)
	/* 0xc1 = 16bit, 5-6-5 RGB */
	necwab_outb(GD542X_REG_3C6, 0xc1);
#elif defined(USE_8BIT_RGB)
	/* 0xc9 = 8bit 3-3-2 RGB */
	necwab_outb(GD542X_REG_3C6, 0xc9);
#else
	/* 0x00 = 256 color map (VGA compat.) */
	necwab_outb(GD542X_REG_3C6, 0x00);
#endif
	nec_cirrus_set_default_cmap();
}

/*
 * set default VGA 256 color map
 */
void
nec_cirrus_set_default_cmap(void) {
	int i;
	u_int8_t red, green, blue;

	necwab_outb(GD542X_REG_3C8, 0);	/* reset */
	for(i = 0; i < sizeof(color_list) / sizeof(color_list[0]); i++) {
		red   = (color_list[i] & 0xff0000) >> 16;
		green = (color_list[i] & 0x00ff00) >>  8;
		blue  = (color_list[i] & 0x0000ff);
		/* data (R,G,B) */
		necwab_outb(GD542X_REG_3C9, red   >> 2);
		necwab_outb(GD542X_REG_3C9, green >> 2);
		necwab_outb(GD542X_REG_3C9, blue  >> 2);
	}
}

/*
 * draw a box
 */
void
draw_box(int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	u_int32_t addr;

	addr = y1 * 1024 + x1;

	for (y = y1; y <= y2; y++)
		for (x = x1; x <= x2; x++) {
			addr = y * 1024 + x;
			nec_cirrus_write(addr, color);
		}
	return;
}
