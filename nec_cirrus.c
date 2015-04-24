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

#ifdef LINEAR	/* not worked :-< */

	/* GRB <- 0x00: single-banking */
	necwab_outw(GD542X_REG_3CE, 0x000B);

	/* linear mapping at 0x00f00000 */
	necwab_outb(GD542X_REG_3C4, 0x07);
	data = necwab_inb(GD542X_REG_3C5);
	data |= 0xf1;		/* map at 0x00f00000, use 256 colors */
	necwab_outb(GD542X_REG_3C5, data);

	addr = (u_int8_t *)(pc98membase + 0x00f00000);

	for (i = 0; i < 0xffff; i++) {
		*(addr + i) = i % 256;
	}

	addr = (u_int8_t *)pc98membase;

	for (i = 0; i < 16; i++)
		for (j = 0; j < 0xfffff; j++) {
			data = *(addr + 0x00100000 * i + j);
			if (data == 0x01)
				printf("found, i = 0x%02x, j = 0x%04x\n", i, j);
		}

#else	/* LINEAR */
	addr = (u_int8_t *)(pc98membase + 0x00f00000);
	for (i = 0; i < 0xffff; i++) {
		*(addr + i) = i  % 256;
	}
#endif

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

void
nec_cirrus_chip_init(void)
{
	int i;
	static int rst_SR[] = {	0x1206, 0x0200, 0x0300, 0x2101, 0x0012 };
#if 0	/* cir_pc98.c */
	static int set_SR[] = {	0x0300, 0x0101, 0x0F02, 0x0003, 0x0E04, 
				/* cirrus extension? */
				0x1206, 0x0107, 0x0008, 0x300F, 
				0x0012, 0xD316, 0x0018, 0x1c1F,
				0x660B, 0x3B1B, 0x480C, 0x231C, 
				0x560D, 0x3D1D, 0x5B0E, 0x3F1E };
	/* CRT controller register */
	static int set_CR[] = {	0x6500, 0x4F01, 0x5002, 0x8A03, 
				0x5904, 0x8105, 0xB606, 0x1F07,
				0x0008, 0x4009, 0x000A, 0x000B,
				0x000C, 0x000D, 0x800E, 0x200F,
				0x9610, 0x8E11, 0x8F12, 0x8013,
				0x0014, 0x8F15, 0x9516, 0xE317,
				0xFF18, 0x3219, 0x501A, 0x221B};
#else
	static int set_SR[] = {	0x0300, 0x0101, 0x0F02, 0x0003, 0x0E04, 
				0x1206, /* cirrus unlock */
				0x0107, /* 256 colors */
				0x0008, /* not use EEPROM */
				0x300F, /* CRT FIFO depth=16, Data bus=32bit */
				0x0012, /* do not use Graphic cursor */
				0xD316,
				0x0018, /* do not use signature register */
				0x1c1F,
				0x660B, /* VCLK0 = 25.180MHz (numetator)*/
				0x3B1B,	/* VCLK0 = 25.180MHz (denominator) */
				0x480C, /* VCLK1 = */
				0x231C, /* VCLK1 = */
				0x560D, /* VCLK2 = */
				0x3D1D,	/* VCLK2 = */
				0x5B0E, /* VCLK3 = */
				0x3F1E	/* VCLK3 = */
				 };
	/* CRT controller register */
	static int set_CR[] = {	0x5F00, 0x4F01, 0x5002, 0x8203,
				0x5404, 0x8005, 0xBF06, 0x1F07,
				0x0008, 0x4109, 0x000A, 0x000B,
				0x000C, 0x000D, 0x000E, 0x000F,
				0x9C10, 0x8E11, 0x8F12, 0x2813,
				0x4014, 0x9615, 0xB916, 0xA317,
				0xFF18, 
				/* cirrus exteinsion? */
				0x3219, /* interlace end register */
				0x501A,
				0x221B
				};
#endif
	/* graphic controller register */
	static int set_GR[] = {	0x0000, 0x0001, 0x0002, 0x0003, 0x0004,
				0x4005, 0x0506, 0x0F07, 0xFF08 };
	/* attribute register */
	static char set_AR[]= {	0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
				0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
				0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00,
				0x0F, 0x00, 0x00 };
	static int set_GR2[] = {0x0009, 0x000a, 0x000b };

	for(i = 0; i < sizeof(rst_SR) / sizeof(rst_SR[0]); i++)
		necwab_outw(GD542X_REG_3C4, rst_SR[i]);
	for(i = 0; i < sizeof(set_SR) / sizeof(set_SR[0]); i++)
		necwab_outw(GD542X_REG_3C4, set_SR[i]);
	necwab_outw(GD542X_REG_3C4, 0x340f);

	/* 400 line, upper page, reserved, 25.175Mhz, RAM enable, color */
	necwab_outb(GD542X_REG_3C2, 0x63);

	/* GR6 <- 0x05: a0000 - 64KB, normal, graphic mode */
	necwab_outw(GD542X_REG_3CE, 0x0506);

	/* SR0 <- 0x03: turn on sequencer */
	necwab_outw(GD542X_REG_3C4, 0x0300);

	/* CR11 <- 0x20: unlock CR0-7 */
	necwab_outw(GD542X_REG_3D4, 0x2011);
	for(i = 0; i < sizeof(set_CR) / sizeof(set_CR[0]); i++)
		necwab_outw(GD542X_REG_3D4, set_CR[i]);

	/* CR1D <- 0x00: */
	necwab_outw(GD542X_REG_3D4, 0x001d);

	/* SR17 <- 0x01: Shadow DAC Write off ?*/
	necwab_outw(GD542X_REG_3C4, 0x0117);

	for(i = 0; i < sizeof(set_GR) / sizeof(set_GR[0]); i++)
		necwab_outw(GD542X_REG_3CE, set_GR[i]);

	necwab_inb(GD542X_REG_3DA);

	for(i = 0 ; i < sizeof(set_AR) / sizeof(set_AR[0]); i++) {
		necwab_outb(GD542X_REG_3C0, i);
		necwab_outb(GD542X_REG_3C0, set_AR[i]);
	}

	/* ARX <- 0x20: Video Enable */
	necwab_outb(GD542X_REG_3C0, 0x20);

	/* HDR <- 0xff: color mode reserved? TRM 9-80 */
	necwab_outb(GD542X_REG_3C6, 0xff);

	for(i = 0; i < sizeof(set_GR2) / sizeof(set_GR2[0]); i++)
		necwab_outw(GD542X_REG_3CE, set_GR2[i]);

	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
	necwab_inb(GD542X_REG_3C6);
}
