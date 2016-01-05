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
#define LINEAR
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "gd54xx.h"
#include "necwab.h"
#include "nec_cirrus.h"

extern u_int32_t color_list[256];

/* prototype */
struct nec_cirrus_config_t *nec_cirrus_chip_init(int);
void	nec_cirrus_dump(u_int32_t);
void	nec_cirrus_enter(void);
void	nec_cirrus_leave(void);
void	nec_cirrus_set_base(u_int8_t);
void	nec_cirrus_set_default_cmap(void);

void	melco_wgna_enter(void);
void	melco_wgna_leave(void);
void	melco_wgna_adjust(struct cbus_gd54xx_sc *, int);

void	draw_box(struct nec_cirrus_config_t *, int, int, int, int, int);

/* register address definition for PC-9801-96 */
struct cbus_gd54xx_sc nec_cirrus_sc = {
	.reg3C0 = 0x0C50,
	.reg3C1 = 0x0C51,
	.reg3C2 = 0x0C52,
	.reg3C3	= 0x0C53,
	.reg3C4 = 0x0C54,
	.reg3C5	= 0x0c55,
	.reg3C6 = 0x0C56,
	.reg3C7 = 0x0C57,
	.reg3C8 = 0x0C58,
	.reg3C9 = 0x0C59,
	.reg3CA = 0x0C5A,
	.reg3CC = 0x0C5C,
	.reg3CE = 0x0C5E,
	.reg3CF = 0x0C5F,
	.reg3D4 = 0x0D54,
	.reg3D5 = 0x0D55,
	.reg3DA	= 0x0D5A,
	.reg102 = 0x0902,
	.reg094 = 0x0904,

	.enter = nec_cirrus_enter,
	.leave = nec_cirrus_leave,
};

/* register address definition for MELCO W[GS]N-A */
struct cbus_gd54xx_sc melco_wgna_sc = {
	.reg3C0 = 0x40e0,
	.reg3C1 = 0x41e0,
	.reg3C2 = 0x42e0,
	.reg3C3	= 0x43e0,
	.reg3C4 = 0x44e0,
	.reg3C5	= 0x45e0,
	.reg3C6 = 0x46e0,
	.reg3C7 = 0x47e0,
	.reg3C8 = 0x48e0,
	.reg3C9 = 0x49e0,
	.reg3CA = 0x4ae0,
	.reg3CC = 0x4ce0,
	.reg3CE = 0x4ee0,
	.reg3CF = 0x4fe0,
	.reg3D4 = 0x54e0,
	.reg3D5 = 0x55e0,
	.reg3DA	= 0x5ae0,
	.reg102 = 0x42e0,
	.reg40E1 = 0x40e1,
	.reg46E8 = 0x46e8,

	.enter = melco_wgna_enter,
	.leave = melco_wgna_leave,
};

struct cbus_gd54xx_sc *cgs;

static u_int8_t nec_cirrus_SRdata[] = {
	0x00,0x02,0x00,0x03,0x01,0x01,0x02,0xFF,
	0x03,0x00,0x04,0x0E,0x05,0x0F,0x06,0x12,
	0x07,0x01,0x08,0x00,0x09,0x6C,0x0A,0x59,
	0x0B,0x4A,0x0C,0x5B,0x0D,0x42,0x0E,0x54,
	0x0F,0xBD,0x10,0x00,0x11,0x00,0x12,0x00,
	0x13,0x3F,0x14,0x00,0x15,0x00,0x16,0x01,
	0x17,0x39,0x18,0x02,0x19,0x01,0x1A,0x00,
	0x1B,0x2B,0x1C,0x2F,0x1D,0x1F,0x1E,0x3B,
	0x1F,0x1F,0xFF
};

static u_int8_t nec_cirrus_CRdata_640[] = {
	0x00,0x67,0x01,0x4f,0x02,0x54,0x03,0x9a,
	0x04,0x55,0x05,0x1a,0x06,0x07,0x07,0x3e,
	0x08,0x00,0x09,0x40,0x0a,0x00,0x0b,0x00,
	0x0c,0x00,0x0d,0x00,0x0e,0x00,0x0f,0x00,
	0x10,0xe8,0x11,0x2b,0x12,0xdf,0x13,0x50,
	0x14,0x00,0x15,0xe8,0x16,0xec,0x17,0xc3,
	0x18,0xff,0x19,0xff,0x1a,0x50,0x1b,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_800[] = {
	0x00,0x7F,0x01,0x63,0x02,0x64,0x03,0x82,
	0x04,0x6A,0x05,0x1A,0x06,0x72,0x07,0xF0,
	0x08,0x00,0x09,0x60,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0x58,0x11,0x8C,0x12,0x57,0x13,0x64,
	0x14,0x00,0x15,0x58,0x16,0x72,0x17,0xE3,
	0x18,0xFF,0x19,0x00,0x1A,0x00,0x1B,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_1024[] = {
	0x00,0xA1,0x01,0x7F,0x02,0x80,0x03,0x85,
	0x04,0x85,0x05,0x96,0x06,0x24,0x07,0xFD,
	0x08,0x00,0x09,0x60,0x0A,0x00,0x0B,0x00,
	0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,
	0x10,0x02,0x11,0x00,0x12,0xFF,0x13,0x80,
	0x14,0x00,0x15,0x00,0x16,0x26,0x17,0xE3,
	0x18,0xFF,0x19,0x32,0x1A,0xE0,0x1B,0x22,
	0xFF
};

static u_int8_t nec_cirrus_CRdata_1280[] = {
	0x00,0x63,0x01,0x4f,0x02,0x51,0x03,0x98,
	0x04,0x52,0x05,0x18,0x06,0x12,0x07,0xb2,
	0x08,0x00,0x09,0x60,0x0a,0x00,0x0b,0x00,
	0x0c,0x00,0x0d,0x00,0x0e,0x00,0x0f,0x00,
	0x10,0x01,0x11,0x23,0x12,0xff,0x13,0xa0,
	0x14,0x00,0x15,0x01,0x16,0x04,0x17,0xc7,
	0x18,0xff,0x19,0xff,0x1a,0x90,0x1b,0x22,
	0xFF
};

static u_int8_t nec_cirrus_GRdata[] = {
	0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,
	0x04,0x00,0x05,0x00,0x06,0x05,0x07,0x0F,
	0x08,0xFF,0x09,0x7F,0x0A,0x80,0x0B,0x2C,
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
		.freq_num   = 0x42,
		.freq_denom = 0x1f
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
	},
	{	/* 3: 1280x1024, V 60Hz, H 49kHz */
		.width  = 1280,
		.height = 1024,
		.depth  = 8,
		.CRdata = nec_cirrus_CRdata_1280,
		.freq_num   = 0x42,	/* 134.998MHz */
		.freq_denom = 0x0e
	}
};

int
nec_cirrus_main(struct board_type_t *bt, int mode)
{
	int i, j, boxw, boxh, linewidth;
	struct nec_cirrus_config_t *ncc;

	switch (bt->type) {
	case 0x60:	/* PC-9801-96 */
		cgs = &nec_cirrus_sc;
		/* set VRAM window address to 0xf00000-0xf0ffff */
		necwab_outb(NECWAB_INDEX, 0x01);
		necwab_outb(NECWAB_DATA, 0xa0);
		break;
	case 0xc2:	/* MELCO WGN/WSN-A */
		melco_wgna_adjust(&melco_wgna_sc, bt->offset);
		cgs = &melco_wgna_sc;
		break;
	default:
		return -1;
	}

	wab_membase = pc98membase + 0x00f00000;

	/* enable WAB chip register */
	cgs->enter();

	printf("Chip ID = 0x%02x\n", nec_cirrus_chip_id());

	ncc = nec_cirrus_chip_init(mode);

	/* clear all 4MB VRAM */
#ifdef LINEAR
	for (i = 0; i < 0x400000; i++) {
		nec_cirrus_linear_write(i, 0);
	}
#else
	for (i = 0; i < 64; i++) {		/* 4MB = 64KB x 64 */
		nec_cirrus_set_base(i << 2);
		memset(wab_membase, 0, 0x10000);	/* fill 64KB window */
	}
#endif

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
#if 0
	for (i = 1; i < 16; i++) {
		nec_cirrus_dump(0x00100000 * i);
		printf("\n");
	}
#endif

	getchar();

	cgs->leave();

	return 0;
}

int
nec_cirrus_chip_id(void)
{
	u_int8_t data, partstatus;

	necwab_outb(cgs->reg3D4, 0x27);
	data = necwab_inb(cgs->reg3D5);

	necwab_outb(cgs->reg3D4, 0x25);
	partstatus = necwab_inb(cgs->reg3D5);

	printf("ID = 0x%02x, Rev = 0x%02x, PartStatus = 0x%02x\n",
		data >> 2, data & 0x03, partstatus);

	data >>= 2;	/* bit 7-2 has Chip ID */
	return data;
}

/*
 * set base register (with simple cache)
 */
void
nec_cirrus_set_base(u_int8_t base) {
	static u_int8_t base0 = 0xff;	/* cache */

	if (base0 != base) {
		necwab_outb(cgs->reg3CE, 0x09);
		necwab_outb(cgs->reg3CF, base);
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

	base   = (u_int8_t)((addr & 0x003f0000) >> 14);
	offset = (u_int32_t)(addr & 0x0000ffff);	/* 64K */
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

	base   = (u_int8_t)((addr & 0x003f0000) >> 14);
	offset = (u_int32_t)(addr & 0x0000ffff);	/* 64K */
#if 0
	printf("0x%02x, 0x%04x\n", base, offset);
#endif
	nec_cirrus_set_base(base);

	raddr = (u_int8_t *)(wab_membase + offset);
	return *raddr;
}

/*
 * write 8bit data to vram address (linear mode)
 */
void
nec_cirrus_linear_write(u_int32_t addr, u_int8_t value)
{
	u_int8_t *waddr;

	waddr = (u_int8_t *)(wab_membase + addr);
	*waddr = value;
}

/*
 * read 8bit data from vram address (linear mode)
 */
u_int8_t
nec_cirrus_linear_read(u_int32_t addr)
{
	u_int8_t *raddr;

	raddr = (u_int8_t *)(wab_membase + addr);
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
#ifdef LINEAR
		printf("%02x ", nec_cirrus_linear_read(vram_addr + i));
#else
		printf("%02x ", nec_cirrus_read(vram_addr + i));
#endif
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
	necwab_outb(cgs->reg3C4, 0x06);
	necwab_outb(cgs->reg3C5, 0x12);
}

/*
 * lock Cirrus extension
 */
void
nec_cirrus_lock(void)
{
	/* 0x00 -> SR6 */ 
	necwab_outb(cgs->reg3C4, 0x06);
	necwab_outb(cgs->reg3C5, 0x00);
}

void
nec_cirrus_enter(void)
{
	/* enable WAB registers & video output */
	necwab_outb(NECWAB_INDEX, 0x03);
	necwab_outb(NECWAB_DATA, 0x03);

	necwab_outb(cgs->reg094, 0x01);
	necwab_outb(cgs->reg102, 0x01);
	necwab_outb(cgs->reg3C2, 0x01);
	necwab_outb(cgs->reg094, 0x20);
	necwab_outb(cgs->reg3C2, 0xe1);
	necwab_outb(cgs->reg3DA, 0x00);
}

void
nec_cirrus_leave(void)
{
	necwab_outb(cgs->reg3C0, 0x1f);
	necwab_inb(cgs->reg3C6);	/* dummy read 4 times to access */
	necwab_inb(cgs->reg3C6);
	necwab_inb(cgs->reg3C6);
	necwab_inb(cgs->reg3C6);
	necwab_outb(cgs->reg3C6, 0x80);

	/* disable WAB registers & video output */
	necwab_outb(NECWAB_INDEX, 0x03);
	necwab_outb(NECWAB_DATA, 0x00);
}

void
melco_wgna_adjust(struct cbus_gd54xx_sc *cgs, int offset)
{
	cgs->reg3C0 = cgs->reg3C0 + offset;
	cgs->reg3C1 = cgs->reg3C1 + offset;
	cgs->reg3C2 = cgs->reg3C2 + offset;
	cgs->reg3C3 = cgs->reg3C3 + offset;
	cgs->reg3C4 = cgs->reg3C4 + offset;
	cgs->reg3C5 = cgs->reg3C5 + offset;
	cgs->reg3C6 = cgs->reg3C6 + offset;
	cgs->reg3C7 = cgs->reg3C7 + offset;
	cgs->reg3C8 = cgs->reg3C8 + offset;
	cgs->reg3C9 = cgs->reg3C9 + offset;
	cgs->reg3CA = cgs->reg3CA + offset;
	cgs->reg3CC = cgs->reg3CC + offset;
	cgs->reg3CE = cgs->reg3CE + offset;
	cgs->reg3CF = cgs->reg3CF + offset;
	cgs->reg3D4 = cgs->reg3D4 + offset;
	cgs->reg3D5 = cgs->reg3D5 + offset;
	cgs->reg3DA = cgs->reg3DA + offset;
	cgs->reg102 = cgs->reg102 + offset;
	cgs->reg40E1 = cgs->reg40E1 + offset;
	/* cgs->reg46E8 does not use with an offset!! */
}

void
melco_wgna_enter(void)
{
	u_int8_t data;

	/* activate */
	necwab_outb(cgs->reg46E8, 0x18);
	necwab_outb(cgs->reg102, 0x01);
	necwab_outb(cgs->reg46E8, 0x08);

	/* enable video output */
	necwab_outb(cgs->reg3C0, 0x00);	/* ARX bit 5: video disable */
	necwab_outb(cgs->reg40E1, 0x7a);	/* graphic board output off */
	data = necwab_inb(cgs->reg3CC);
	necwab_outb(cgs->reg3C2, data | 0x02);	/* enable display memory */
	necwab_outb(cgs->reg40E1, 0x7b);	/* graphic board output on */
	necwab_outb(cgs->reg3C0, 0x20);	/* ARX bit 5: video enable */

	necwab_outb(cgs->reg3C4, 0x0f);
	if (necwab_inb(0x5be1) & 0x08)
		necwab_outb(cgs->reg3C5, 0x7d); /* 2M bytes model */
	else
		necwab_outb(cgs->reg3C5, 0xfd); /* 4M bytes model */
}

void
melco_wgna_leave(void)
{
	u_int8_t data;

	necwab_outb(cgs->reg3C0, 0x1f);
	necwab_inb(cgs->reg3C6);	/* dummy read 4 times to access */
	necwab_inb(cgs->reg3C6);
	necwab_inb(cgs->reg3C6);
	necwab_inb(cgs->reg3C6);
	necwab_outb(cgs->reg3C6, 0x80);

	/* disable video output */
	data = necwab_inb(cgs->reg3CC);
	necwab_outb(cgs->reg3C2, data & 0xfd);	/* disable display memory */
	necwab_outb(cgs->reg40E1, 0x7a);	/* display: to normal */
	necwab_outb(cgs->reg3C0, 0x00);	/* ARX bit 5: video disable */
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
	necwab_outb(cgs->reg3C2, 0xef);
	/* enable VGA subsystem */
	necwab_outb(cgs->reg3C3, 0x01);

	/* SR: Sequence Control Register */
	for (c = nec_cirrus_SRdata; *c != 0xff; /* empty */) {
		necwab_outb(cgs->reg3C4, *c++);
		necwab_outb(cgs->reg3C5, *c++);
	}

	nec_cirrus_CRdata = ncc->CRdata;

	necwab_outb(cgs->reg3C4, 0x0e);
	necwab_outb(cgs->reg3C5, ncc->freq_num);
	necwab_outb(cgs->reg3C4, 0x1e);
	necwab_outb(cgs->reg3C5, ncc->freq_denom);

	/* CR */
	/* unlock CR0-7 */
	necwab_outb(cgs->reg3D4, 0x11);
	necwab_outb(cgs->reg3D5, 0x00);

	for (c = nec_cirrus_CRdata; *c != 0xff; /* empty */) {
		necwab_outb(cgs->reg3D4, *c++);
		necwab_outb(cgs->reg3D5, *c++);
	}

	/* GR */
	for (c = nec_cirrus_GRdata; *c != 0xff; /* empty */) {
		necwab_outb(cgs->reg3CE, *c++);
		necwab_outb(cgs->reg3CF, *c++);
	}

	if (ncc->depth == 16) {
		/* SR */
#if 0
		/* color mode */
		necwab_outb(cgs->reg3C4, 0x0f);
		necwab_outb(cgs->reg3C5, 0x95);
#endif
		/* clock: 16bit/pixel data at pixel rate */
		necwab_outb(cgs->reg3C4, 0x07);
		necwab_outb(cgs->reg3C5, 0x07);

		/* CR */
		necwab_outb(cgs->reg3D4, 0x13);
		if ((mode  & 0x0f) == 0)	/* 640x480 */
			necwab_outb(cgs->reg3D5, 0xa0);
		else if ((mode  & 0x0f) == 1)	/* 800x600 */
			necwab_outb(cgs->reg3D5, 0xc8);
		else if ((mode  & 0x0f) == 2) {	/* 1024x768 */
			u_int8_t data;
			necwab_outb(cgs->reg3D5, 0x00);
			necwab_outb(cgs->reg3D4, 0x1b);
			data = necwab_inb(cgs->reg3D5);
			necwab_outb(cgs->reg3D5, data | 0x10);
		}
#if 0
		/* GR */
		/* not necessary? current value is 0x0c on both 8/16 bit */
		/* GRB: Enable enhanced writes for 16-bit pixels */
		necwab_outb(cgs->reg3CE, 0x0b);
		necwab_outb(cgs->reg3CF, 0x1c);
#endif
	}

#ifdef LINEAR
	{
		u_int8_t data;
		/* use 2MB: see GD543X TRM Appendix D2-4 */
		necwab_outb(cgs->reg3CE, 0x0b);
		data = necwab_inb(cgs->reg3CF);
		necwab_outb(cgs->reg3CF, data | 0x20);

		/* Linear mapping at 0x00e00000 */
		necwab_outb(cgs->reg3C4, 0x07);
		data = necwab_inb(cgs->reg3C5);
		necwab_outb(cgs->reg3C5, data | 0xe0);
		wab_membase = pc98membase + 0x00e00000;
	}
#endif

	/* AR */
	/* clear internal flip-flop status for AR */
	necwab_inb(cgs->reg3DA);
	for (c = nec_cirrus_ARdata; *c != 0xff; /* empty */) {
		necwab_outb(cgs->reg3C0, *c++);
		necwab_outb(cgs->reg3C0, *c++);
	}
	necwab_inb(cgs->reg3DA);
	/* ARX <- 0x20: Video Enable */
	necwab_outb(cgs->reg3C0, 0x20);

	/* pixel mask register <- 0xff: color mode through */
	necwab_outb(cgs->reg3C6, 0xff);

	/* HDR: Hidden DAC Register */
	necwab_inb(cgs->reg3DA);
	necwab_inb(cgs->reg3C6);	/* four times dummy reads */
	necwab_inb(cgs->reg3C6);
	necwab_inb(cgs->reg3C6);
	necwab_inb(cgs->reg3C6);
	if (ncc->depth == 16) {
		/* 0xc1 = 16bit, 5-6-5 RGB */
		necwab_outb(cgs->reg3C6, 0xc1);
	} else {
#ifdef USE_8BIT_RGB
		/* 0xc9 = 8bit 3-3-2 RGB */
		necwab_outb(cgs->reg3C6, 0xc9);
#else
		if (mode == 3) {
			/* TRM 9-95: if VCLK > 85MHz, 5434 uses special mode */
			necwab_outb(cgs->reg3C6, 0x4a);
			/* also, clock is 16bit/pixel data at pixel rate */
			necwab_outb(cgs->reg3C4, 0x07);
			necwab_outb(cgs->reg3C5, 0x07);
		} else
			/* 0x00 = 256 color map (VGA compat.) */
			necwab_outb(cgs->reg3C6, 0x00);
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

	necwab_outb(cgs->reg3C8, 0);	/* reset */
	for(i = 0; i < sizeof(color_list) / sizeof(color_list[0]); i++) {
		red   = (color_list[i] & 0xff0000) >> 16;
		green = (color_list[i] & 0x00ff00) >>  8;
		blue  = (color_list[i] & 0x0000ff);
		/* data (R,G,B) */
		necwab_outb(cgs->reg3C9, red   >> 2);
		necwab_outb(cgs->reg3C9, green >> 2);
		necwab_outb(cgs->reg3C9, blue  >> 2);
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
#ifdef LINEAR
				nec_cirrus_linear_write(addr, color & 0xff);
				nec_cirrus_linear_write(addr + 1,
				    (color & 0xff00) >> 8);
#else
				nec_cirrus_write(addr, color & 0xff);
				nec_cirrus_write(addr + 1,
				    (color & 0xff00) >> 8);
#endif
			} else {
				addr = y * ncc->width + x;
#ifdef LINEAR
				nec_cirrus_linear_write(addr, color);
#else
				nec_cirrus_write(addr, color);
#endif
			}
		}
	return;
}
