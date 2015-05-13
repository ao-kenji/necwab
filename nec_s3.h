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
 * register address definition for PC-9801-85
 * From : XFree86/xc/programs/Xserver/hw/xfree98/accel/s3nec/s3pc98.c
 */

/*
 * original VGA ports 0x3B0 - 0x3DF are mapped on 0x1B0 -- 0x1DF
 */
#define S3_928_REG_3C0	0x01C0	/* Attribute Controller Index (ARX) */
#define S3_928_REG_3C1	0x01C1	/* Attribute Controller Data */
#define S3_928_REG_3C2	0x01C2	/* W:Misc. Output / Input Status Register 0 */
#define S3_928_REG_3C3	0x01C3	/* Motherboard Sleep */
#define S3_928_REG_3C4	0x01C4	/* VGA Sequencer Index (SRX) */
#define S3_928_REG_3C5	0x01C5	/* VGA Sequencer Data */
#define S3_928_REG_3C6	0x01C6	/* Pixel Mask */
#define S3_928_REG_3C7	0x01C7	/* W:Pixel Address(Read mode) / R:DAC State */
#define S3_928_REG_3C8	0x01C8	/* Pixel Address (Write mode) */
#define S3_928_REG_3C9	0x01C9	/* Pixel Data */
#define S3_928_REG_3CA	0x01CA	/* R:Feature Control */
#define S3_928_REG_3CC	0x01CC	/* R:Misc. Output */
#define S3_928_REG_3CE	0x01CE	/* Graphics Controller Index (GRX) */
#define S3_928_REG_3CF	0x01CF	/* Graphics Controller Data */
#define S3_928_REG_3D4	0x01D4	/* CRTC Index (CRX) */
#define S3_928_REG_3D5	0x01D5	/* CRTC Data */
#define S3_928_REG_3DA	0x01DA	/* W:Feature Control / Input Status Register 1 */


#define S3_928_REG_102	0x0002
#define S3_928_REG_042	0x0240
#define S3_928_REG_046  0x0244
#define S3_928_REG_04A  0x0248
#define S3_928_REG_082  0x0280
#define S3_928_REG_086  0x0284
#define S3_928_REG_08A  0x0288
#define S3_928_REG_08E  0x028c
#define S3_928_REG_092  0x0290
#define S3_928_REG_096  0x0294
#define S3_928_REG_09A  0x0298
#define S3_928_REG_09E  0x029c
#define S3_928_REG_0A2  0x02a0
#define S3_928_REG_0A6  0x02a4
#define S3_928_REG_0AA  0x02a8
#define S3_928_REG_0AE  0x02ac
#define S3_928_REG_0B2  0x02b0
#define S3_928_REG_0B6  0x02b4
#define S3_928_REG_0BA  0x02b8
#define S3_928_REG_0BE  0x02bc
#define S3_928_REG_0E2  0x02e0

/*
 * prototypes
 */

int	nec_s3_main(int);
void	nec_s3_reg_on(void);
void	nec_s3_reg_off(void);
void	nec_s3_disp_on(void);
void	nec_s3_disp_off(void);
void	nec_s3_unlock(void);
void	nec_s3_lock(void);
