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
 * CL-GD54xx structure
 */

struct cbus_gd54xx_sc {
	u_int16_t	reg3C0;	/* Attribute Controller Index (ARX) */
	u_int16_t	reg3C1;	/* Attribute Controller Data */
	u_int16_t	reg3C2;	/* W:Misc. Output / Input Status Register 0 */
	u_int16_t	reg3C3;	/* Motherboard Sleep */
	u_int16_t	reg3C4;	/* VGA Sequencer Index (SRX) */
	u_int16_t	reg3C5;	/* VGA Sequencer Data */
	u_int16_t	reg3C6;	/* Pixel Mask */
	u_int16_t	reg3C7;	/* W:Pixel Address(Read mode) / R:DAC State */
	u_int16_t	reg3C8;	/* Pixel Address (Write mode) */
	u_int16_t	reg3C9;	/* Pixel Data */
	u_int16_t	reg3CA;	/* R:Feature Control */
	u_int16_t	reg3CC;	/* R:Misc. Output */
	u_int16_t	reg3CE;	/* Graphics Controller Index (GRX) */
	u_int16_t	reg3CF;	/* Graphics Controller Data */
	u_int16_t	reg3D4;	/* CRTC Index (CRX) */
	u_int16_t	reg3D5;	/* CRTC Data */
	u_int16_t	reg3DA;	/* W:Feat. Control / Input Status Register 1 */
	u_int16_t	reg102;
	u_int16_t	reg094;
	u_int16_t	reg40E1;
	u_int16_t	reg46E8;

	void		(*enter)(void);	/* board dependent function */
	void		(*leave)(void);	/* board dependent function */
};
