/*-
 * Copyright (c) 2008 Stanislav Sedov <stas@FreeBSD.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: releng/11.0/usr.sbin/cpucontrol/cpucontrol.h 181430 2008-08-08 16:26:53Z stas $
 */

#ifndef CPUCONTROL_H
#define	CPUCONTROL_H

typedef int ucode_probe_t(int fd);
typedef void ucode_update_t(const char *dev, const char *image);

extern int verbosity_level;

#ifdef DEBUG
# define WARNX(level, ...)					\
	if ((level) <= verbosity_level) {			\
		fprintf(stderr, "%s:%d ", __FILE__, __LINE__);	\
		warnx(__VA_ARGS__);				\
	}
# define WARN(level, ...)					\
	if ((level) <= verbosity_level) {			\
		fprintf(stderr, "%s:%d ", __FILE__, __LINE__);	\
		warn(__VA_ARGS__);				\
	}
#else
# define WARNX(level, ...)					\
	if ((level) <= verbosity_level)				\
		warnx(__VA_ARGS__);
# define WARN(level, ...)					\
	if ((level) <= verbosity_level)				\
		warn(__VA_ARGS__);
#endif

#endif /* !CPUCONTROL_H */
