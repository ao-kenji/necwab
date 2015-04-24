# Makefile

PROG = necwab
SRCS = main.c necwab.c nec_cirrus.c nec_s3.c
BINDIR = /home/aoyama/bin
NOMAN = 1

.include <bsd.prog.mk>
