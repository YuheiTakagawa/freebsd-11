# $FreeBSD: releng/11.0/lib/clang/libllvmmipsasmprinter/Makefile 292988 2015-12-31 14:39:45Z dim $

.include <bsd.own.mk>

LIB=	llvmmipsasmprinter

SRCDIR=	lib/Target/Mips/InstPrinter
INCDIR=	lib/Target/Mips
SRCS=	MipsInstPrinter.cpp

TGHDRS=	Attributes \
	MipsGenAsmWriter \
	MipsGenInstrInfo \
	MipsGenRegisterInfo \
	MipsGenSubtargetInfo

.include "../clang.lib.mk"
