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
	int type;

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
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 0) {
		usage();
		return 1;
	}

	type = necwab_init();
	switch (type) {
	case 0x20:
	case 0x21:
		nec_s3_main();
		break;
	case 0x60:
		nec_cirrus_main();
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
	printf("Usage:\n");
}
