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

#include "nec_cirrus.h"
#include "necwab.h"
#include "color.h"

static u_int8_t nec_cirrus_SRdata[] = {
	0x00,0x02,0x00,0x03,0x01,0x01,0x02,0xFF,
	0x03,0x00,0x04,0x0E,0x05,0x0F,0x06,0x12,
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
	0x10,0xEA,0x11,0x8C,0x12,0xDF,0x13,0x50,	/* CR13: A0 -> 50 */
	0x14,0x00,0x15,0xE7,0x16,0x04,0x17,0xE3,
	0x18,0xFF,0x19,0x00,0x1A,0x00,0x1B,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_800[] = {
	0x00,0x7F,0x01,0x63,0x02,0x64,0x03,0x82,
	0x04,0x6A,0x05,0x1A,0x06,0x72,0x07,0xF0,
	0x08,0x00,0x09,0x60,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0x58,0x11,0x8C,0x12,0x57,0x13,0x64,	/* CR13: C8 -> 64 */
	0x14,0x00,0x15,0x58,0x16,0x72,0x17,0xE3,
	0x18,0xFF,0x19,0x00,0x1A,0x00,0x1B,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_1024[] = {
	0x00,0xA1,0x01,0x7F,0x02,0x80,0x03,0x85,
	0x04,0x85,0x05,0x96,0x06,0x24,0x07,0xFD,
	0x08,0x00,0x09,0x60,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,	/* CRE: 08 -> 00, CRF: 20 -> 00 */
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
	0x08,0xFF,0x09,0x7F,0x0A,0x80,0x0B,0x0C,	/* GRB 0E -> 0C */
	0x0C,0xFF,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0xFF,0x11,0x00,0x12,0xFF,0x13,0xFF,
	0x14,0x00,0x15,0x00,0xFF
};

static u_int8_t nec_cirrus_ARdata[] = {
	0x10,0x41,0x11,0x00,0x12,0x0F,0x13,0x00,
	0x14,0x00,0xFF
};
struct nec_cirrus_config_t {
	int width;
	int height;
	int depth;
	u_int8_t *CRdata;
	u_int8_t freq_num;
	u_int8_t freq_denom;
};

struct nec_cirrus_config_t nec_cirrus_config[] = {
	{	/* 0: 640x480, V 60Hz, H 32kHz */
		.width  = 640,
		.height = 480,
		.depth  = 8,
		.CRdata = nec_cirrus_CRdata_640,
		.freq_num   = 0x65,
		.freq_denom = 0x3b
	},
	{	/* 1: 800x600, V 60Hz, H 38kHz */
		.width  = 800,
		.height = 600,
		.depth  = 8,
		.CRdata = nec_cirrus_CRdata_800,
		.freq_num   = 0x5f,
		.freq_denom = 0x23
	},
	{	/* 2: 1024x768, V 60Hz, H 49kHz */
		.width  = 1024,
		.height = 768,
		.depth  = 8,
		.CRdata = nec_cirrus_CRdata_1024,
		.freq_num   = 0x4d,
		.freq_denom = 0x22
	}
};

struct nec_cirrus_config_t *nec_cirrus_chip_init(int);
void	nec_cirrus_dump(u_int32_t);
void	nec_cirrus_enter(void);
void	nec_cirrus_leave(void);
void	nec_cirrus_set_base(u_int8_t);
void	nec_cirrus_set_default_cmap(void);

void	draw_box(struct nec_cirrus_config_t *, int, int, int, int, int);

int
nec_cirrus_main(int mode)
{
	int i, j, boxw, boxh, linewidth;
	struct nec_cirrus_config_t *ncc;

	/* set VRAM window address to 0xf00000-0xf0ffff */
	necwab_outb(NECWAB_INDEX, 0x01);
	necwab_outb(NECWAB_DATA, 0xa0);
	wab_membase = pc98membase + 0x00f00000;

	/* enable WAB chip register */
	nec_cirrus_enter();

	printf("Chip ID = 0x%02x\n", nec_cirrus_chip_id());

	ncc = nec_cirrus_chip_init(mode);

	/* clear all 1MB VRAM */
	for (i = 0; i < 0xfffff; i++) {
		nec_cirrus_write(i, 0);
	}

	if (ncc->depth == 16) {
		/* 16bit depth: write 256x256 matrix */
		boxw = ncc->width  / 256;
		boxh = ncc->height / 256;
		for (j = 0; j < 256; j++)
			for (i = 0; i < 256; i++)
				draw_box(ncc, i * boxw, j * boxh,
				    (i + 1) * boxw - 1, (j + 1) * boxh - 1,
				    j * 256 + i);
	} else {
		/* write 256 color matrix */
		boxw = ncc->width  / 16;
		boxh = ncc->height / 16;
		for (j = 0; j < 16; j++)
			for (i = 0; i < 16; i++)
				draw_box(ncc, i * boxw, j * boxh,
				    (i + 1) * boxw - 2, (j + 1) * boxh - 2,
				    j * 16 + i);
	}

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
	static u_int8_t base0 = 0xff;	/* cache */

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
	necwab_outb(GD542X_REG_3C4, 0x06);
	necwab_outb(GD542X_REG_3C5, 0x12);
}

/*
 * lock Cirrus extension
 */
void
nec_cirrus_lock(void)
{
	/* 0x00 -> SR6 */ 
	necwab_outb(GD542X_REG_3C4, 0x06);
	necwab_outb(GD542X_REG_3C5, 0x00);
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

struct nec_cirrus_config_t *
nec_cirrus_chip_init(int mode)
{
	int i, index;
	u_int8_t *c, *nec_cirrus_CRdata;
	struct nec_cirrus_config_t *ncc;

	ncc = &nec_cirrus_config[mode & 0x0f];
	if (mode & 0xf0)
		ncc->depth = 16;

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

	nec_cirrus_CRdata = ncc->CRdata;

	necwab_outb(GD542X_REG_3C4, 0x0e);
	necwab_outb(GD542X_REG_3C5, ncc->freq_num);
	necwab_outb(GD542X_REG_3C4, 0x1e);
	necwab_outb(GD542X_REG_3C5, ncc->freq_denom);

	/* CR */
	/* unlock CR0-7 */
	necwab_outb(GD542X_REG_3D4, 0x11);
	necwab_outb(GD542X_REG_3D5, 0x00);

	for (c = nec_cirrus_CRdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3D4, *c++);
		necwab_outb(GD542X_REG_3D5, *c++);
	}

	/* GR */
	for (c = nec_cirrus_GRdata; *c != 0xff; /* empty */) {
		necwab_outb(GD542X_REG_3CE, *c++);
		necwab_outb(GD542X_REG_3CF, *c++);
	}

	if (ncc->depth == 16) {
		/* SR */
		/* clock: 16bit/pixel data at pixel rate */
		necwab_outb(GD542X_REG_3C4, 0x07);
		necwab_outb(GD542X_REG_3C5, 0x07);
		/* color mode */
		necwab_outb(GD542X_REG_3C4, 0x0f);
		necwab_outb(GD542X_REG_3C5, 0x95);
		necwab_outb(GD542X_REG_3C4, 0x07);
		necwab_outb(GD542X_REG_3C5, 0x07);

		/* CR */
		necwab_outb(GD542X_REG_3D4, 0x13);
		if ((mode  & 0x0f) == 0)	/* 640x480 */
			necwab_outb(GD542X_REG_3D5, 0xa0);
		else if ((mode  & 0x0f) == 1)	/* 800x600 */
			necwab_outb(GD542X_REG_3D5, 0xc8);

#if 0
		/* GR */
		/* not necessary? current value is 0x0c on both 8/16 bit */
		/* GRB: Enable enhanced writes for 16-bit pixels */
		necwab_outb(GD542X_REG_3CE, 0x0b);
		necwab_outb(GD542X_REG_3CF, 0x1c);
#endif
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
	necwab_inb(GD542X_REG_3C6);	/* four times dummy reads */
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	if (ncc->depth == 16) {
		/* 0xc1 = 16bit, 5-6-5 RGB */
		necwab_outb(GD542X_REG_3C6, 0xc1);
	} else {
#ifdef USE_8BIT_RGB
		/* 0xc9 = 8bit 3-3-2 RGB */
		necwab_outb(GD542X_REG_3C6, 0xc9);
#else
		/* 0x00 = 256 color map (VGA compat.) */
		necwab_outb(GD542X_REG_3C6, 0x00);
#endif
	}
	nec_cirrus_set_default_cmap();

	return ncc;
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
draw_box(struct nec_cirrus_config_t *ncc,
	int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	u_int32_t addr;

	for (y = y1; y <= y2; y++)
		for (x = x1; x <= x2; x++) {
			if (ncc->depth == 16) {
				addr = (y * ncc->width + x) * 2;
				nec_cirrus_write(addr, color & 0xff);
				nec_cirrus_write(addr + 1,
				    (color & 0xff00) >> 8);
			} else {
				addr = y * ncc->width + x;
				nec_cirrus_write(addr, color);
			}
		}
	return;
}
