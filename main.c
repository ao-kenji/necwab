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
 * NEC Windows Accelerator Board test on OpenBSD/luna88k
 */

#include <stdio.h>
#include <stdlib.h>	/* getprogname(3) */
#include <unistd.h>	/* getopt(3) */
#include <sys/ioctl.h>

#include "necwab.h"
#include "nec_cirrus.h"
#include "nec_s3.h"

void	usage(void);

/* global */
int	debug;

int
main(int argc, char **argv)
{
	int type, mode;
	const char *errstr;

	/*
	 * parse options
	 */
	int ch;
	extern char *optarg;
	extern int optind, opterr;

	while ((ch = getopt(argc, argv, "dr:")) != -1) {
		switch (ch) {
		case 'd':	/* debug flag */
			debug = 1;
			break;
		default:
			usage();
			return 1;
			/* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	if (argc == 0)
		mode = 2;
	else if (argc == 1)
		mode = (int)strtonum(argv[0], 0, 63, &errstr);

	switch (mode) {
	case 0x0:	/* 640x480, 8bpp */
	case 0x1:	/* 800x600, 8bpp */
	case 0x2:	/* 1024x768, 8bpp (default) */
	/* case 0x3:	   1280x1024, 8bpp */
	case 0x10:	/* 640x480, 16bpp(5-6-5) */
	case 0x11:	/* 800x600, 16bpp(5-6-5) */
	/* case 0x12:	   1024x768, 16bpp(5-6-5) */
		break;
	default:
		usage();
		return 1;
	}

	type = necwab_init();
	switch (type) {
	case 0x20:
	case 0x21:
		nec_s3_main(2);
		break;
	case 0x60:
		nec_cirrus_main(mode);
		break;
	default:
		break;
	}

	necwab_fini();
	return 0;
}

void
usage(void)
{
	extern char *__progname;

	printf("Usage: %s [mode]\n", __progname);
	printf("\tmode:\t 0: 640x480x8bpp,   1: 800x600x8bpp,  2: 1024x768x8bpp(default)\n");
	printf("\t\t16: 640x480x16bpp, 17: 800x600x16bpp\n");
}
