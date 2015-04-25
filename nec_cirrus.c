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

	addr = (u_int8_t *)(pc98membase + 0x00f00000);
	for (i = 0; i < 0x0fff; i++) {
		if (i % 2 == 0)
			*(addr + i) = ((i / 2) % 256);
		else
			/* attribute, omit blinking */
			*(addr + i) = ((i / 2) % 256);
	}

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
	0x00,	/* 07: reset ext sequence (?) */
	0x00,	/* 08: ??? */
	0x5c,	/* 09: ??? */
	0x09,	/* 0A: BIOS Scratch register for 542x (?) */
	0x4a,	/* 0B: ??? */
	0x5b,	/* 0C: ??? */
	0x42,	/* 0D: VCLK2 frequency */
	0x00,	/* 0E: VCLK3 frequency */
	0x09,	/* 0F: ??? */
	0x00,	/* 10: ??? */
	0x00,	/* 11: ??? */
	0x00,	/* 12: ??? */
	0x00,	/* 13: ??? */
	0x00,	/* 14: BIOS scratch register for 546x (?) */
	0x00,	/* 15: ??? */
	0xd8,	/* 16: ??? */
	0x39,	/* 17: ??? */
	0x00,	/* 18: ??? */
	0x01,	/* 19: ??? */
	0x00,	/* 1A: ??? */
	0x2b,	/* 1B: ??? */
	0x2f,	/* 1C: ??? */
	0x1f,	/* 1D: VCLK2 denominator and post-scalar value */
	0x00,	/* 1E: VCLK3 denominator and post-scalar value */
	0x19	/* 1F: MCLK (?) */
};

void
nec_cirrus_chip_init(void)
{
	int i;

	/* use as standard VGA */

	/* pixel mask register <- 0xff: color mode through */
	necwab_outb(GD542X_REG_3C6, 0xff);

	/* 400 line, upper page, reserved, 25.175Mhz, RAM enable, color */
	necwab_outb(GD542X_REG_3C2, 0x63);

	for(i = 0; i < sizeof(mode03_SR) / sizeof(mode03_SR[0]); i++) {
		necwab_outb(GD542X_REG_3C4, (u_int8_t)i);
		necwab_outb(GD542X_REG_3C5, mode03_SR[i]);
	}

#if 0
	nec_cirrus_unlock();
	for(i = 0; i < sizeof(vga_gd54xx) / sizeof(vga_gd54xx[0]); i++) {
		necwab_outb(GD542X_REG_3C4, (u_int8_t)(i + 5));
		necwab_outb(GD542X_REG_3C5, vga_gd54xx[i]);
	}
	nec_cirrus_lock();
#endif

	/* CR11 <- 0x20; unlock CR0-7 */
	necwab_outw(GD542X_REG_3D4, 0x2011);

	for(i = 0; i < sizeof(mode03_CR) / sizeof(mode03_CR[0]); i++) {
		necwab_outb(GD542X_REG_3D4, (u_int8_t)i);
		necwab_outb(GD542X_REG_3D5, mode03_CR[i]);
	}

	for(i = 0; i < sizeof(mode03_GR) / sizeof(mode03_GR[0]); i++) {
		necwab_outb(GD542X_REG_3CE, (u_int8_t)i);
		necwab_outb(GD542X_REG_3CF, mode03_GR[i]);
	}
	/* a0000 - 64KB, normal, text mode */
	necwab_outb(GD542X_REG_3CE, 0x06);
	necwab_outb(GD542X_REG_3CF, 0x06);

	/* clear internal flip-flop status for AR */
	necwab_inb(GD542X_REG_3DA);
	for(i = 0; i < sizeof(mode03_AR) / sizeof(mode03_AR[0]); i++) {
		necwab_outb(GD542X_REG_3C0, (u_int8_t)i);
		necwab_outb(GD542X_REG_3C0, mode03_AR[i]);
	}
	necwab_inb(GD542X_REG_3DA);
	/* ARX <- 0x20: Video Enable */
	necwab_outb(GD542X_REG_3C0, 0x20);

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
#if 0
	necwab_outw(GD542X_REG_3C4, 0x340f);

	/* CR1D <- 0x00: */
	necwab_outw(GD542X_REG_3D4, 0x001d);

	necwab_inb(GD542X_REG_3DA);

	/* GR6 <- 0x05: a0000 - 64KB, normal, graphic mode */
	necwab_outw(GD542X_REG_3CE, 0x0506);

	for(i = 0; i < sizeof(set_GR2) / sizeof(set_GR2[0]); i++)
		necwab_outw(GD542X_REG_3CE, set_GR2[i]);

	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
#endif
}
