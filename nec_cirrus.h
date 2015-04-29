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
 * register address definition for PC-9801-96
 */

#define GD542X_REG_3C0	0x0C50	/* Attribute Controller Index (ARX) */
#define GD542X_REG_3C1	0x0C51	/* Attribute Controller Data */
#define GD542X_REG_3C2	0x0C52	/* W:Misc. Output / Input Status Register 0 */
#define GD542X_REG_3C3	0x0C53	/* Motherboard Sleep */
#define GD542X_REG_3C4	0x0C54	/* VGA Sequencer Index (SRX) */
#define GD542X_REG_3C5	0x0c55	/* VGA Sequencer Data */
#define GD542X_REG_3C6	0x0C56	/* Pixel Mask */
#define GD542X_REG_3C7	0x0C57	/* W:Pixel Address(Read mode) / R:DAC State */
#define GD542X_REG_3C8	0x0C58	/* Pixel Address (Write mode) */
#define GD542X_REG_3C9	0x0C59	/* Pixel Data */
#define GD542X_REG_3CA	0x0C5A	/* R:Feature Control */
#define GD542X_REG_3CC	0x0C5C	/* R:Misc. Output */
#define GD542X_REG_3CE	0x0C5E	/* Graphics Controller Index (GRX) */
#define GD542X_REG_3CF	0x0C5F	/* Graphics Controller Data */
#define GD542X_REG_3D4	0x0D54	/* CRTC Index (CRX) */
#define GD542X_REG_3D5	0x0D55	/* CRTC Data */
#define GD542X_REG_3DA	0x0D5A	/* W:Feature Control / Input Status Register 1 */
#define GD542X_REG_102	0x0902
#define GD542X_REG_094	0x0904

/*
 * prototypes
 */

int	nec_cirrus_main(int);
void	nec_cirrus_reg_on(void);
void	nec_cirrus_reg_off(void);
void	nec_cirrus_disp_on(void);
void	nec_cirrus_disp_off(void);
void	nec_cirrus_unlock(void);
void	nec_cirrus_lock(void);
void	nec_cirrus_write(u_int32_t, u_int8_t);
u_int8_t	nec_cirrus_read(u_int32_t);
