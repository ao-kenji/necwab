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

#include "nec_s3.h"
#include "necwab.h"

extern u_int32_t color_list[256];

static u_int8_t nec_s3_SRdata[] = {
	0x00,0x03,0x01,0x01,0x02,0x0f,0x03,0x00,
	0x04,0x0e,
	0xff
};

/* 1024x768 */
static u_int8_t nec_s3_CRdata[] = {
	0x00,0xa1, 0x01,0x7f, 0x02,0x80, 0x03,0x84,
	0x04,0x84, 0x05,0x95, 0x06, 0x24, 0x07,0xe5, /* 00 - 07 */
	0x08,0x00, 0x09,0x60, 0x0a,0x00, 0x0b,0x00,
	0x0c,0x00, 0x0d,0x00, 0x0e,0xff, 0x0f,0x00, /* 08 - 0F */
	0x10,0x02, 0x11,0x88, 0x12,0xff, 0x13,0x80,
	0x14,0x60, 0x15,0xff, 0x16,0x24, 0x17,0xe3, /* 10 - 17 */
	0x18,0xff, 0x19,0xff, 0x1a,0xff, 0x1b,0xff,
	0x1c,0xff, 0x1d,0xff, 0x1e,0xff, 0x1f,0xff, /* 18 - 1F */
	0x20,0xff, 0x21,0x00, 0x22,0x00, 0x23,0x00,
	0x24,0x00, 0x25,0x00, 0x26,0x00, 0x27,0x00, /* 20 - 27 */
	0x28,0x00, 0x29,0x00, 0x2a,0x00, 0x2b,0x00,
	0x2c,0x00, 0x2d,0x00, 0x2e,0x00, 0x2f,0x00, /* 28 - 2F */
	0x30,0xc0, 0x31,0x8d, 0x32,0x00, 0x33,0x00,
	0x34,0x10, 0x35,0x30, 0x36,0x8d, 0x37,0xfd, /* 30 - 37 */
	0x38,0x79, 0x39,0xbd, 0x3a,0x95, 0x3b,0x9a,
	0x3c,0x00, 0x3d,0x00, 0x3e,0x00, 0x3f,0x00, /* 38 - 3F */
	0x40,0x01, 0x41,0x00, 0x42,0x02, 0x43,0x00,
	0x44,0x00, 0x45,0x00, 0x46,0x00, 0x47,0xff, /* 40 - 47 */
	0x48,0xfc, 0x49,0xff, 0x4a,0xff, 0x4b,0x00,
	0x4c,0x1f, 0x4d,0xff, 0x4e,0xc0, 0x4f,0xc0, /* 48 - 4F */
	0x50,0x00, 0x51,0x00, 0x52,0x00, 0x53,0x00,
	0x54,0x50, 0x55,0x00, 0x56,0x00, 0x57,0x00, /* 50 - 57 */
	0x58,0x0a, 0x59,0x00, 0x5a,0x00, 0x5b,0x00,
	0x5c,0x02, 0x5d,0x00, 0x5e,0x00, 0x5f,0x00, /* 58 - 5F */
	0x60,0xff, 0x61,0x80, 0x62,0x80, 0x63,0x00,
	0x64,0x00, 0x65,0x00, 0x66,0x00, 0x67,0x00, /* 60 - 67 */
	0x68,0xff, 0x69,0x00, 0x6a,0x00, 0x6b,0x00,
	0x6c,0x00, 0x6d,0x00, 0x6e,0x00, 0x6f,0x00, /* 68 - 6F */
	0xff
};

static u_int8_t nec_s3_GRdata[] = {
	0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,
	0x04,0x00,0x05,0x00,0x06,0x05,0x07,0x0f,
	0x08,0xff,
	0xff
};

#if 0
static u_int8_t nec_s3_ARdata[] = {
	0x10,0x41,0x11,0x00,0x12,0x0F,0x13,0x00,
	0x14,0x00,0xFF
};
#endif

/* MCLK = 62.56 MHz   m = 0x7f, n = 0x2d */
static unsigned char sdac_data[] = {
	0x28, 0x61, 0x3d, 0x62, 0x28, 0x22, 0x77, 0x4a,	0x79, 0x49,
	0x62, 0x46, 0x6b, 0x2a, 0x52, 0x26, 0x52, 0x26, 0x52, 0x26,
	0x7f, 0x2d, 0x7f, 0x2d,	0x7f, 0x2d, 0x7f, 0x2d, 0x22, 0x22,
	0xff
};

struct nec_s3_config_t {
	int width;
	int height;
	int depth;
	u_int8_t *CRdata;
	u_int8_t freq_num;
	u_int8_t freq_denom;
};

struct nec_s3_config_t nec_s3_config[] = {
	{	/* 0: 640x480, V 60Hz, H 32kHz */
		.width  = 640,
		.height = 480,
		.depth  = 8,
		.CRdata = nec_s3_CRdata,	/* XXX */
		.freq_num   = 0x65,
		.freq_denom = 0x3b
	},
	{	/* 1: 800x600, V 60Hz, H 38kHz */
		.width  = 800,
		.height = 600,
		.depth  = 8,
		.CRdata = nec_s3_CRdata,	/* XXX */
		.freq_num   = 0x5f,
		.freq_denom = 0x23
	},
	{	/* 2: 1024x768, V 60Hz, H 49kHz */
		.width  = 1024,
		.height = 768,
		.depth  = 8,
		.CRdata = nec_s3_CRdata,	/* XXX */
		.freq_num   = 0x4d,
		.freq_denom = 0x22
	}
};

struct nec_s3_config_t *nec_s3_chip_init(int);
void	nec_s3_dump(u_int32_t);
void	nec_s3_enter(void);
void	nec_s3_leave(void);
void	nec_s3_set_default_cmap(void);
#if 0
void	nec_s3_draw_box(struct nec_cirrus_config_t *, int, int, int, int, int);
#endif
extern int wab_iofd, wab_memfd;
extern u_int8_t *pc98iobase, *pc98membase;
extern u_int8_t *wab_iobase,*wab_membase;

int
nec_s3_main(int mode)
{
	int i, j;
	struct nec_s3_config_t *nsc;

	/* memory mapped I/O = 0x000d0000 */
	necwab_outb(NECWAB_INDEX, 0x01);
	necwab_outb(NECWAB_DATA, 0x5f);	/* 0xNN0df000-0xNN0dffff */
	necwab_outb(NECWAB_INDEX, 0x02);
	necwab_outb(NECWAB_DATA, 0x00);	/* specify above NN */
	wab_iobase = pc98membase + 0x000df000;

	/* enable WAB chip register */
	nec_s3_enter();

	nsc = nec_s3_chip_init(2);

#if 0
	/* clear all 1MB VRAM */
	for (i = 0; i < 0xfffff; i++) {
		nec_cirrus_write(i, 0);
	}
#endif

	/* do something */

	getchar();

	nec_s3_leave();

	return 0;
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

#if 0
/*
 * 256 bytes vram memory dump
 */
void
nec_s3_dump(u_int32_t vram_addr)
	int i;

	for (i = 0; i < 256; i++) {
		if (i % 16 == 0)
			printf("0x%08x ", vram_addr + i);
		printf("%02x ", nec_s3_read(vram_addr + i));
		if (i % 16 == 15)
			printf("\n");
	}
}
#endif

/*
 * unlock S3 extension
 */
void
nec_s3_unlock(void)
{
	/* CR38 and CR39 */
	necwab_outb(S3_928_REG_3D4, 0x38);	/* 3?5H */
	necwab_outb(S3_928_REG_3D5, 0x48);
	necwab_outb(S3_928_REG_3D4, 0x39);
	necwab_outb(S3_928_REG_3D5, 0xa0);
	necwab_outb(S3_928_REG_3D4, 0x40);
	necwab_outb(S3_928_REG_3D5, 0xa5);
}

/*
 * lock S3 extension
 */
void
nec_s3_lock(void)
{
#if 0
	/* 0x00 -> SR6 */
	necwab_outb(GD542X_REG_3C4, 0x06);
	necwab_outb(GD542X_REG_3C5, 0x00);
#endif
}

void
nec_s3_enter(void)
{
	/* enable WAB registers & video output */
	necwab_outb(NECWAB_INDEX, 0x03);
	necwab_outb(NECWAB_DATA, 0x03);

#if 0
	necwab_outb(0x46, 0x1e);	/* ROM_PAGE_SEL */
#endif
	necwab_outb(S3_928_REG_102, 0x01);
#if 0
	necwab_outb(0x46, 0x0e);	/* ROM_PAGE_SEL */
#endif
	/*
	 * bit0: enable enhanced functions
	 * bit1: reserved = 1
	 * bit2: screen size (1 = 1024x768 or 800x600)
	 * bit3: reserved
	 * bit4: LA; linear addressing (1=on, same as bit4 of CR58)
	 * bit5: MMIO; memory mapped I/O (1=on, same as bit4 of CR53)
	 * bit6: WP; write posting into FIFO (1=on, same as bit3 of CR40)
	 */
	necwab_outb(0x4a, 0x0005);	/* ADVFUNC_CNTL, 0x0005 */

	necwab_outb(S3_928_REG_3C2, 0xcd);
	necwab_outb(S3_928_REG_3DA, 0x00);

	/* enable S3 VGA Register group */
	necwab_outb(S3_928_REG_3D4, 0x38);
	necwab_outb(S3_928_REG_3D5, 0x48);

	/* enable System Control and System Extension Register group */
	necwab_outb(S3_928_REG_3D4, 0x39);
	necwab_outb(S3_928_REG_3D5, 0xa0);

	/* enable Enhanced Graphic Command group */
	necwab_outb(S3_928_REG_3D4, 0x40);
	necwab_outb(S3_928_REG_3D5, 0xa5);
}

void
nec_s3_leave(void)
{
#if 0
	necwab_outb(GD542X_REG_3C0, 0x1f);
	necwab_inb(GD542X_REG_3C6);	/* dummy read 4 times to access */
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_outb(GD542X_REG_3C6, 0x80);
#endif

	/* disable WAB registers & video output */
	necwab_outb(NECWAB_INDEX, 0x03);
	necwab_outb(NECWAB_DATA, 0x00);
}

struct nec_s3_config_t *
nec_s3_chip_init(int mode)
{
	int i, index;
	u_int8_t *c, data;
	struct nec_s3_config_t *nsc;

	nsc = &nec_s3_config[mode & 0x0f];
	if (mode & 0xf0)
		nsc->depth = 16;

	/* should be called after nec_s3_enter() */

	/* MISC */
	necwab_outb(S3_928_REG_3C2, 0xef);
	/* enable VGA subsystem */
	necwab_outb(S3_928_REG_3C3, 0x01);

	/* SDAC */
	necwab_outb(S3_928_REG_3D4, 0x55);
	data = necwab_inb(S3_928_REG_3D5);
	data |= 0x01;
	necwab_outb(S3_928_REG_3D5, data);
	necwab_outb(S3_928_REG_3C8, 0);
	for (c = sdac_data; *c != 0xff; /* empty */) {
		necwab_outb(S3_928_REG_3C9, *c++);
	}

	/* SR: Sequence Control Register */
	for (c = nec_s3_SRdata; *c != 0xff; /* empty */) {
		necwab_outb(S3_928_REG_3C4, *c++);
		necwab_outb(S3_928_REG_3C5, *c++);
	}

#if 0
	nec_s3_CRdata = nsc->CRdata;
#endif

#if 0
	necwab_outb(GD542X_REG_3C4, 0x0e);
	necwab_outb(GD542X_REG_3C5, ncc->freq_num);
	necwab_outb(GD542X_REG_3C4, 0x1e);
	necwab_outb(GD542X_REG_3C5, ncc->freq_denom);
#endif

	/* CR */
	/* unlock CR0-7 */
	necwab_outb(S3_928_REG_3D4, 0x11);
	necwab_outb(S3_928_REG_3D5, 0x00);

	for (c = nec_s3_CRdata; *c != 0xff; /* empty */) {
		necwab_outb(S3_928_REG_3D4, *c++);
		necwab_outb(S3_928_REG_3D5, *c++);
	}

	/* GR */
	for (c = nec_s3_GRdata; *c != 0xff; /* empty */) {
		necwab_outb(S3_928_REG_3CE, *c++);
		necwab_outb(S3_928_REG_3CF, *c++);
	}

#if 0
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
#endif	/* of 16BIT */

#if 0
	/* AR */
	/* clear internal flip-flop status for AR */
	necwab_inb(S3_928_REG_3DA);
	for (c = nec_s3_ARdata; *c != 0xff; /* empty */) {
		necwab_outb(S3_928_REG_3C0, *c++);
		necwab_outb(S3_928_REG_3C0, *c++);
	}
	necwab_inb(S3_928_REG_3DA);
	/* ARX <- 0x20: Video Enable */
	necwab_outb(S3_928_REG_3C0, 0x20);
#endif

	/* pixel mask register <- 0xff: color mode through */
	necwab_outb(S3_928_REG_3C6, 0xff);

	nec_s3_set_default_cmap();

	return nsc;
}

/*
 * set default VGA 256 color map
 */
void
nec_s3_set_default_cmap(void) {
	int i;
	u_int8_t red, green, blue;

	necwab_outb(S3_928_REG_3C8, 0);	/* reset */
	for(i = 0; i < sizeof(color_list) / sizeof(color_list[0]); i++) {
		red   = (color_list[i] & 0xff0000) >> 16;
		green = (color_list[i] & 0x00ff00) >>  8;
		blue  = (color_list[i] & 0x0000ff);
		/* data (R,G,B) */
		necwab_outb(S3_928_REG_3C9, red   >> 2);
		necwab_outb(S3_928_REG_3C9, green >> 2);
		necwab_outb(S3_928_REG_3C9, blue  >> 2);
	}
}

inline u_int8_t
nec_s3_inb(u_int16_t index)
{
	return *(wab_iobase + index);
}

inline void
nec_s3_outb(u_int16_t index, u_int8_t data)
{
	*(wab_iobase + index) = data;
}
