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

/*
 * mode XX: 640x480x16color
 *  based on http://community.osdev.info/?VGA
 *  based on XXXXX
 */

static char modeXX_GeneralR = 0x63;

static char modeXX_SR[] = {
	0x03, 0x01, 0x0f, 0x00, 0x06
};

static char modeXX_CR[] = {
	0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0x0b, 0x3e,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xea, 0x8c, 0xdf, 0x28, 0x00, 0xe7, 0x04, 0xe3,
	0xff
};

static char modeXX_GR[] = {
	0x00, 0x0f, 0x00, 0x00, 0x00, 0x03, 0x05, 0x00,
	0xff
};

static char modeXX_AR[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x01, 0x00, 0x0f, 0x00, 0x00
};

/*
 * mode 03+: Text, 16 color, 80x25, 720x400
 */

static char mode03_GeneralR = 0x63;

static char mode03_SR[] = {
	0x03, 0x01, 0x03, 0x00, 0x02
};

static char mode03_CR[] = {
	0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
	0x00, 0xc7, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
	0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
	0xff
};

static char mode03_GR[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00,
	0xff
};

static char mode03_AR[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x08, 0x00, 0x0f, 0x00, 0x00
};

/*
 * mode 12: Graphic, 16 color, 80x30, 640x480
 */

static char mode12_GeneralR = 0xe3;

static char mode12_SR[] = {
	0x03, 0x01, 0x0f, 0x00, 0x06
};

static char mode12_CR[] = {
	0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x3e,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xea, 0x8c, 0xdf, 0x28, 0x00, 0xe7, 0x04, 0xe3,
	0xff
};

static char mode12_GR[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f,
	0xff
};

static char mode12_AR[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x01, 0x00, 0x0f, 0x00, 0x00
};

struct necwab_vga_init_regs {
	char *vga_GeneralR;
	char **vga_SR;
	char **vga_CR;
	char **vga_GR;
	char **vga_AR;
};

/*
 * palette
 */

struct necwab_pal {
	u_int8_t r;
	u_int8_t g;
	u_int8_t b;
} default_pal[] = {
	/* red, green, blue */
        {   0x00,   0x00,   0x00,   },
        {   0x00,   0x00,   0x2A,   },
        {   0x00,   0x2A,   0x00,   },
        {   0x00,   0x2A,   0x2A,   },
        {   0x2A,   0x00,   0x00,   },
        {   0x2A,   0x00,   0x2A,   },
        {   0x2A,   0x2A,   0x00,   },
        {   0x2A,   0x2A,   0x2A,   },
        {   0x15,   0x15,   0x15,   },
        {   0x15,   0x15,   0x3F,   },
        {   0x15,   0x3F,   0x15,   },
        {   0x15,   0x3F,   0x3F,   },
        {   0x3F,   0x15,   0x15,   },
        {   0x3F,   0x15,   0x3F,   },
        {   0x3F,   0x3F,   0x15,   },
        {   0x3F,   0x3F,   0x3F,   },
};
