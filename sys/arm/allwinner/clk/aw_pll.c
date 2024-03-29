/*-
 * Copyright (c) 2016 Jared McNeill <jmcneill@invisible.ca>
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
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: releng/11.0/sys/arm/allwinner/clk/aw_pll.c 299703 2016-05-13 22:28:02Z gonzo $
 */

/*
 * Allwinner PLL clock
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: releng/11.0/sys/arm/allwinner/clk/aw_pll.c 299703 2016-05-13 22:28:02Z gonzo $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/rman.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <machine/bus.h>

#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>
#include <dev/ofw/ofw_subr.h>

#include <dev/extres/clk/clk.h>

#include <dt-bindings/clock/sun4i-a10-pll2.h>

#include "clkdev_if.h"

#define	AW_PLL_ENABLE			(1 << 31)

#define	A10_PLL1_OUT_EXT_DIVP		(0x3 << 16)
#define	A10_PLL1_OUT_EXT_DIVP_SHIFT	16
#define	A10_PLL1_FACTOR_N		(0x1f << 8)
#define	A10_PLL1_FACTOR_N_SHIFT		8
#define	A10_PLL1_FACTOR_K		(0x3 << 4)
#define	A10_PLL1_FACTOR_K_SHIFT		4
#define	A10_PLL1_FACTOR_M		(0x3 << 0)
#define	A10_PLL1_FACTOR_M_SHIFT		0

#define	A10_PLL2_POST_DIV		(0xf << 26)
#define	A10_PLL2_POST_DIV_SHIFT		26
#define	A10_PLL2_FACTOR_N		(0x7f << 8)
#define	A10_PLL2_FACTOR_N_SHIFT		8
#define	A10_PLL2_PRE_DIV		(0x1f << 0)
#define	A10_PLL2_PRE_DIV_SHIFT		0

#define	A10_PLL3_MODE_SEL		(0x1 << 15)
#define	A10_PLL3_MODE_SEL_FRACT		(0 << 15)
#define	A10_PLL3_MODE_SEL_INT		(1 << 15)
#define	A10_PLL3_FUNC_SET		(0x1 << 14)
#define	A10_PLL3_FUNC_SET_270MHZ	(0 << 14)
#define	A10_PLL3_FUNC_SET_297MHZ	(1 << 14)
#define	A10_PLL3_FACTOR_M		(0x7f << 0)
#define	A10_PLL3_FACTOR_M_SHIFT		0
#define	A10_PLL3_REF_FREQ		3000000

#define	A10_PLL5_OUT_EXT_DIVP		(0x3 << 16)
#define	A10_PLL5_OUT_EXT_DIVP_SHIFT	16
#define	A10_PLL5_FACTOR_N		(0x1f << 8)
#define	A10_PLL5_FACTOR_N_SHIFT		8
#define	A10_PLL5_FACTOR_K		(0x3 << 4)
#define	A10_PLL5_FACTOR_K_SHIFT		4
#define	A10_PLL5_FACTOR_M1		(0x3 << 2)
#define	A10_PLL5_FACTOR_M1_SHIFT	2
#define	A10_PLL5_FACTOR_M		(0x3 << 0)
#define	A10_PLL5_FACTOR_M_SHIFT		0

#define	A10_PLL6_BYPASS_EN		(1 << 30)
#define	A10_PLL6_SATA_CLK_EN		(1 << 14)
#define	A10_PLL6_FACTOR_N		(0x1f << 8)
#define	A10_PLL6_FACTOR_N_SHIFT		8
#define	A10_PLL6_FACTOR_K		(0x3 << 4)
#define	A10_PLL6_FACTOR_K_SHIFT		4
#define	A10_PLL6_FACTOR_M		(0x3 << 0)
#define	A10_PLL6_FACTOR_M_SHIFT		0

#define	A10_PLL2_POST_DIV		(0xf << 26)

#define	A23_PLL1_FACTOR_N		(0x1f << 8)
#define	A23_PLL1_FACTOR_N_SHIFT		8
#define	A23_PLL1_FACTOR_K		(0x3 << 4)
#define	A23_PLL1_FACTOR_K_SHIFT		4
#define	A23_PLL1_FACTOR_M		(0x3 << 0)
#define	A23_PLL1_FACTOR_M_SHIFT		0
#define	A23_PLL1_FACTOR_P		(0x3 << 16)
#define	A23_PLL1_FACTOR_P_SHIFT		16

#define	A31_PLL1_LOCK			(1 << 28)
#define	A31_PLL1_CPU_SIGMA_DELTA_EN	(1 << 24)
#define	A31_PLL1_FACTOR_N		(0x1f << 8)
#define	A31_PLL1_FACTOR_N_SHIFT		8
#define	A31_PLL1_FACTOR_K		(0x3 << 4)
#define	A31_PLL1_FACTOR_K_SHIFT		4
#define	A31_PLL1_FACTOR_M		(0x3 << 0)
#define	A31_PLL1_FACTOR_M_SHIFT		0

#define	A31_PLL6_LOCK			(1 << 28)
#define	A31_PLL6_BYPASS_EN		(1 << 25)
#define	A31_PLL6_CLK_OUT_EN		(1 << 24)
#define	A31_PLL6_24M_OUT_EN		(1 << 18)
#define	A31_PLL6_24M_POST_DIV		(0x3 << 16)
#define	A31_PLL6_24M_POST_DIV_SHIFT	16
#define	A31_PLL6_FACTOR_N		(0x1f << 8)
#define	A31_PLL6_FACTOR_N_SHIFT		8
#define	A31_PLL6_FACTOR_K		(0x3 << 4)
#define	A31_PLL6_FACTOR_K_SHIFT		4
#define	A31_PLL6_DEFAULT_N		0x18
#define	A31_PLL6_DEFAULT_K		0x1
#define	A31_PLL6_TIMEOUT		10

#define	A80_PLL4_CLK_OUT_EN		(1 << 20)
#define	A80_PLL4_PLL_DIV2		(1 << 18)
#define	A80_PLL4_PLL_DIV1		(1 << 16)
#define	A80_PLL4_FACTOR_N		(0xff << 8)
#define	A80_PLL4_FACTOR_N_SHIFT		8

#define	CLKID_A10_PLL3_1X		0
#define	CLKID_A10_PLL3_2X		1

#define	CLKID_A10_PLL5_DDR		0
#define	CLKID_A10_PLL5_OTHER		1

#define	CLKID_A10_PLL6_SATA		0
#define	CLKID_A10_PLL6_OTHER		1
#define	CLKID_A10_PLL6			2
#define	CLKID_A10_PLL6_DIV_4		3

#define	CLKID_A31_PLL6			0
#define	CLKID_A31_PLL6_X2		1

enum aw_pll_type {
	AWPLL_A10_PLL1 = 1,
	AWPLL_A10_PLL2,
	AWPLL_A10_PLL3,
	AWPLL_A10_PLL5,
	AWPLL_A10_PLL6,
	AWPLL_A23_PLL1,
	AWPLL_A31_PLL1,
	AWPLL_A31_PLL6,
	AWPLL_A80_PLL4,
};

struct aw_pll_sc {
	enum aw_pll_type	type;
	device_t		clkdev;
	bus_addr_t		reg;
	int			id;
};

struct aw_pll_funcs {
	int	(*recalc)(struct aw_pll_sc *, uint64_t *);
	int	(*set_freq)(struct aw_pll_sc *, uint64_t, uint64_t *, int);
	int	(*init)(device_t, bus_addr_t, struct clknode_init_def *);
};

#define	PLL_READ(sc, val)	CLKDEV_READ_4((sc)->clkdev, (sc)->reg, (val))
#define	PLL_WRITE(sc, val)	CLKDEV_WRITE_4((sc)->clkdev, (sc)->reg, (val))
#define	DEVICE_LOCK(sc)		CLKDEV_DEVICE_LOCK((sc)->clkdev)
#define	DEVICE_UNLOCK(sc)	CLKDEV_DEVICE_UNLOCK((sc)->clkdev)

static int
a10_pll1_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, m, n, k, p;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	p = 1 << ((val & A10_PLL1_OUT_EXT_DIVP) >> A10_PLL1_OUT_EXT_DIVP_SHIFT);
	m = ((val & A10_PLL1_FACTOR_M) >> A10_PLL1_FACTOR_M_SHIFT) + 1;
	k = ((val & A10_PLL1_FACTOR_K) >> A10_PLL1_FACTOR_K_SHIFT) + 1;
	n = (val & A10_PLL1_FACTOR_N) >> A10_PLL1_FACTOR_N_SHIFT;
	if (n == 0)
		n = 1;

	*freq = (*freq * n * k) / (m * p);

	return (0);
}

static int
a10_pll2_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, post_div, n, pre_div;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	post_div = (val & A10_PLL2_POST_DIV) >> A10_PLL2_POST_DIV_SHIFT;
	if (post_div == 0)
		post_div = 1;
	n = (val & A10_PLL2_FACTOR_N) >> A10_PLL2_FACTOR_N_SHIFT;
	if (n == 0)
		n = 1;
	pre_div = (val & A10_PLL2_PRE_DIV) >> A10_PLL2_PRE_DIV_SHIFT;
	if (pre_div == 0)
		pre_div = 1;

	switch (sc->id) {
	case SUN4I_A10_PLL2_1X:
		*freq = (*freq * 2 * n) / pre_div / post_div / 2;
		break;
	case SUN4I_A10_PLL2_2X:
		*freq = (*freq * 2 * n) / pre_div / 4;
		break;
	case SUN4I_A10_PLL2_4X:
		*freq = (*freq * 2 * n) / pre_div / 2;
		break;
	case SUN4I_A10_PLL2_8X:
		*freq = (*freq * 2 * n) / pre_div;
		break;
	default:
		return (EINVAL);
	}

	return (0);
}

static int
a10_pll2_set_freq(struct aw_pll_sc *sc, uint64_t fin, uint64_t *fout,
    int flags)
{
	uint32_t val, post_div, n, pre_div;

	if (sc->id != SUN4I_A10_PLL2_1X)
		return (ENXIO);

	/*
	 * Audio Codec needs PLL2-1X to be either 24576000 or 22579200.
	 *
	 * PLL2-1X output frequency is (48MHz * n) / pre_div / post_div / 2.
	 * To get as close as possible to the desired rate, we use a
	 * pre-divider of 21 and a post-divider of 4. With these values,
	 * a multiplier of 86 or 79 gets us close to the target rates.
	 */
	if (*fout != 24576000 && *fout != 22579200)
		return (EINVAL);

	pre_div = 21;
	post_div = 4;
	n = (*fout * pre_div * post_div * 2) / (2 * fin);

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	val &= ~(A10_PLL2_POST_DIV | A10_PLL2_FACTOR_N | A10_PLL2_PRE_DIV);
	val |= (post_div << A10_PLL2_POST_DIV_SHIFT);
	val |= (n << A10_PLL2_FACTOR_N_SHIFT);
	val |= (pre_div << A10_PLL2_PRE_DIV_SHIFT);
	PLL_WRITE(sc, val);
	DEVICE_UNLOCK(sc);

	return (0);
}

static int
a10_pll3_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, m;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	if ((val & A10_PLL3_MODE_SEL) == A10_PLL3_MODE_SEL_INT) {
		/* In integer mode, output is 3MHz * m */
		m = (val & A10_PLL3_FACTOR_M) >> A10_PLL3_FACTOR_M_SHIFT;
		*freq = A10_PLL3_REF_FREQ * m;
	} else {
		/* In fractional mode, output is either 270MHz or 297MHz */
		if ((val & A10_PLL3_FUNC_SET) == A10_PLL3_FUNC_SET_270MHZ)
			*freq = 270000000;
		else
			*freq = 297000000;
	}

	if (sc->id == CLKID_A10_PLL3_2X)
		*freq *= 2;

	return (0);
}

static int
a10_pll3_set_freq(struct aw_pll_sc *sc, uint64_t fin, uint64_t *fout,
    int flags)
{
	uint32_t val, m, mode, func;

	m = *fout / A10_PLL3_REF_FREQ;
	if (sc->id == CLKID_A10_PLL3_2X)
		m /= 2;

	mode = A10_PLL3_MODE_SEL_INT;
	func = 0;
	*fout = m * A10_PLL3_REF_FREQ;
	if (sc->id == CLKID_A10_PLL3_2X)
		*fout *= 2;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	val &= ~(A10_PLL3_MODE_SEL | A10_PLL3_FUNC_SET | A10_PLL3_FACTOR_M);
	val |= mode;
	val |= func;
	val |= (m << A10_PLL3_FACTOR_M_SHIFT);
	PLL_WRITE(sc, val);
	DEVICE_UNLOCK(sc);

	return (0);
}

static int
a10_pll3_init(device_t dev, bus_addr_t reg, struct clknode_init_def *def)
{
	uint32_t val;

	/* Allow changing PLL frequency while enabled */
	def->flags = CLK_NODE_GLITCH_FREE;

	/* Set PLL to 297MHz */
	CLKDEV_DEVICE_LOCK(dev);
	CLKDEV_READ_4(dev, reg, &val);
	val &= ~(A10_PLL3_MODE_SEL | A10_PLL3_FUNC_SET | A10_PLL3_FACTOR_M);
	val |= A10_PLL3_MODE_SEL_FRACT;
	val |= A10_PLL3_FUNC_SET_297MHZ;
	CLKDEV_WRITE_4(dev, reg, val);
	CLKDEV_DEVICE_UNLOCK(dev);

	return (0);
}

static int
a10_pll5_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, m, n, k, p;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	p = 1 << ((val & A10_PLL5_OUT_EXT_DIVP) >> A10_PLL5_OUT_EXT_DIVP_SHIFT);
	m = ((val & A10_PLL5_FACTOR_M) >> A10_PLL5_FACTOR_M_SHIFT) + 1;
	k = ((val & A10_PLL5_FACTOR_K) >> A10_PLL5_FACTOR_K_SHIFT) + 1;
	n = (val & A10_PLL5_FACTOR_N) >> A10_PLL5_FACTOR_N_SHIFT;
	if (n == 0)
		return (ENXIO);

	switch (sc->id) {
	case CLKID_A10_PLL5_DDR:
		*freq = (*freq * n * k) / m;
		break;
	case CLKID_A10_PLL5_OTHER:
		*freq = (*freq * n * k) / p;
		break;
	default:
		return (ENXIO);
	}

	return (0);
}

static int
a10_pll6_init(device_t dev, bus_addr_t reg, struct clknode_init_def *def)
{
	uint32_t val, m, n, k;

	/*
	 * SATA needs PLL6 to be a 100MHz clock.
	 *
	 * The SATA output frequency is (24MHz * n * k) / m / 6.
	 * To get to 100MHz, k & m must be equal and n must be 25.
	 */
	m = k = 0;
	n = 25;

	CLKDEV_DEVICE_LOCK(dev);
	CLKDEV_READ_4(dev, reg, &val);
	val &= ~(A10_PLL6_FACTOR_N | A10_PLL6_FACTOR_K | A10_PLL6_FACTOR_M);
	val &= ~A10_PLL6_BYPASS_EN;
	val |= A10_PLL6_SATA_CLK_EN;
	val |= (n << A10_PLL6_FACTOR_N_SHIFT);
	val |= (k << A10_PLL6_FACTOR_K_SHIFT);
	val |= (m << A10_PLL6_FACTOR_M_SHIFT);
	CLKDEV_WRITE_4(dev, reg, val);
	CLKDEV_DEVICE_UNLOCK(dev);

	return (0);
}

static int
a10_pll6_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, m, k, n;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	m = ((val & A10_PLL6_FACTOR_M) >> A10_PLL6_FACTOR_M_SHIFT) + 1;
	k = ((val & A10_PLL6_FACTOR_K) >> A10_PLL6_FACTOR_K_SHIFT) + 1;
	n = (val & A10_PLL6_FACTOR_N) >> A10_PLL6_FACTOR_N_SHIFT;
	if (n == 0)
		return (ENXIO);

	switch (sc->id) {
	case CLKID_A10_PLL6_SATA:
		*freq = (*freq * n * k) / m / 6;
		break;
	case CLKID_A10_PLL6_OTHER:
		*freq = (*freq * n * k) / 2;
		break;
	case CLKID_A10_PLL6:
		*freq = (*freq * n * k);
		break;
	case CLKID_A10_PLL6_DIV_4:
		*freq = (*freq * n * k) / 4;
		break;
	default:
		return (ENXIO);
	}

	return (0);
}

static int
a10_pll6_set_freq(struct aw_pll_sc *sc, uint64_t fin, uint64_t *fout,
    int flags)
{
	if (sc->id != CLKID_A10_PLL6_SATA)
		return (ENXIO);

	/* PLL6 SATA output has been set to 100MHz in a10_pll6_init */
	if (*fout != 100000000)
		return (ERANGE);

	return (0);
}

static int
a23_pll1_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, m, n, k, p;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	m = ((val & A23_PLL1_FACTOR_M) >> A23_PLL1_FACTOR_M_SHIFT) + 1;
	k = ((val & A23_PLL1_FACTOR_K) >> A23_PLL1_FACTOR_K_SHIFT) + 1;
	n = ((val & A23_PLL1_FACTOR_N) >> A23_PLL1_FACTOR_N_SHIFT) + 1;
	p = ((val & A23_PLL1_FACTOR_P) >> A23_PLL1_FACTOR_P_SHIFT) + 1;

	*freq = (*freq * n * k) / (m * p);

	return (0);
}

static int
a31_pll1_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, m, n, k;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	m = ((val & A31_PLL1_FACTOR_M) >> A31_PLL1_FACTOR_M_SHIFT) + 1;
	k = ((val & A31_PLL1_FACTOR_K) >> A31_PLL1_FACTOR_K_SHIFT) + 1;
	n = ((val & A31_PLL1_FACTOR_N) >> A31_PLL1_FACTOR_N_SHIFT) + 1;

	*freq = (*freq * n * k) / m;

	return (0);
}

static int
a31_pll6_init(device_t dev, bus_addr_t reg, struct clknode_init_def *def)
{
	uint32_t val;
	int retry;

	if (def->id != CLKID_A31_PLL6)
		return (0);

	/*
	 * The datasheet recommends that PLL6 output should be fixed to
	 * 600MHz.
	 */
	CLKDEV_DEVICE_LOCK(dev);
	CLKDEV_READ_4(dev, reg, &val);
	val &= ~(A31_PLL6_FACTOR_N | A31_PLL6_FACTOR_K | A31_PLL6_BYPASS_EN);
	val |= (A31_PLL6_DEFAULT_N << A31_PLL6_FACTOR_N_SHIFT);
	val |= (A31_PLL6_DEFAULT_K << A31_PLL6_FACTOR_K_SHIFT);
	CLKDEV_WRITE_4(dev, reg, val);

	/* Wait for PLL to become stable */
	for (retry = A31_PLL6_TIMEOUT; retry > 0; retry--) {
		CLKDEV_READ_4(dev, reg, &val);
		if ((val & A31_PLL6_LOCK) == A31_PLL6_LOCK)
			break;
		DELAY(1);
	}

	CLKDEV_DEVICE_UNLOCK(dev);

	if (retry == 0)
		return (ETIMEDOUT);

	return (0);
}

static int
a31_pll6_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, k, n;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	k = ((val & A10_PLL6_FACTOR_K) >> A10_PLL6_FACTOR_K_SHIFT) + 1;
	n = ((val & A10_PLL6_FACTOR_N) >> A10_PLL6_FACTOR_N_SHIFT) + 1;

	switch (sc->id) {
	case CLKID_A31_PLL6:
		*freq = (*freq * n * k) / 2;
		break;
	case CLKID_A31_PLL6_X2:
		*freq = *freq * n * k;
		break;
	default:
		return (ENXIO);
	}

	return (0);
}

static int
a80_pll4_recalc(struct aw_pll_sc *sc, uint64_t *freq)
{
	uint32_t val, n, div1, div2;

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	DEVICE_UNLOCK(sc);

	n = (val & A80_PLL4_FACTOR_N) >> A80_PLL4_FACTOR_N_SHIFT;
	div1 = (val & A80_PLL4_PLL_DIV1) == 0 ? 1 : 2;
	div2 = (val & A80_PLL4_PLL_DIV2) == 0 ? 1 : 2;

	*freq = (*freq * n) / div1 / div2;

	return (0);
}

#define	PLL(_type, _recalc, _set_freq, _init)	\
	[(_type)] = {				\
		.recalc = (_recalc),		\
		.set_freq = (_set_freq),	\
		.init = (_init)			\
	}

static struct aw_pll_funcs aw_pll_func[] = {
	PLL(AWPLL_A10_PLL1, a10_pll1_recalc, NULL, NULL),
	PLL(AWPLL_A10_PLL2, a10_pll2_recalc, a10_pll2_set_freq, NULL),
	PLL(AWPLL_A10_PLL3, a10_pll3_recalc, a10_pll3_set_freq, a10_pll3_init),
	PLL(AWPLL_A10_PLL5, a10_pll5_recalc, NULL, NULL),
	PLL(AWPLL_A10_PLL6, a10_pll6_recalc, a10_pll6_set_freq, a10_pll6_init),
	PLL(AWPLL_A23_PLL1, a23_pll1_recalc, NULL, NULL),
	PLL(AWPLL_A31_PLL1, a31_pll1_recalc, NULL, NULL),
	PLL(AWPLL_A31_PLL6, a31_pll6_recalc, NULL, a31_pll6_init),
	PLL(AWPLL_A80_PLL4, a80_pll4_recalc, NULL, NULL),
};

static struct ofw_compat_data compat_data[] = {
	{ "allwinner,sun4i-a10-pll1-clk",	AWPLL_A10_PLL1 },
	{ "allwinner,sun4i-a10-pll2-clk",	AWPLL_A10_PLL2 },
	{ "allwinner,sun4i-a10-pll3-clk",	AWPLL_A10_PLL3 },
	{ "allwinner,sun4i-a10-pll5-clk",	AWPLL_A10_PLL5 },
	{ "allwinner,sun4i-a10-pll6-clk",	AWPLL_A10_PLL6 },
	{ "allwinner,sun6i-a31-pll1-clk",	AWPLL_A31_PLL1 },
	{ "allwinner,sun6i-a31-pll6-clk",	AWPLL_A31_PLL6 },
	{ "allwinner,sun8i-a23-pll1-clk",	AWPLL_A23_PLL1 },
	{ "allwinner,sun9i-a80-pll4-clk",	AWPLL_A80_PLL4 },
	{ NULL, 0 }
};

static int
aw_pll_init(struct clknode *clk, device_t dev)
{
	clknode_init_parent_idx(clk, 0);
	return (0);
}

static int
aw_pll_set_gate(struct clknode *clk, bool enable)
{
	struct aw_pll_sc *sc;
	uint32_t val;

	sc = clknode_get_softc(clk);

	DEVICE_LOCK(sc);
	PLL_READ(sc, &val);
	if (enable)
		val |= AW_PLL_ENABLE;
	else
		val &= ~AW_PLL_ENABLE;
	PLL_WRITE(sc, val);
	DEVICE_UNLOCK(sc);

	return (0);
}

static int
aw_pll_recalc(struct clknode *clk, uint64_t *freq)
{
	struct aw_pll_sc *sc;

	sc = clknode_get_softc(clk);

	if (aw_pll_func[sc->type].recalc == NULL)
		return (ENXIO);

	return (aw_pll_func[sc->type].recalc(sc, freq));
}

static int
aw_pll_set_freq(struct clknode *clk, uint64_t fin, uint64_t *fout,
    int flags, int *stop)
{
	struct aw_pll_sc *sc;

	sc = clknode_get_softc(clk);

	*stop = 1;

	if (aw_pll_func[sc->type].set_freq == NULL)
		return (ENXIO);

	return (aw_pll_func[sc->type].set_freq(sc, fin, fout, flags));
}

static clknode_method_t aw_pll_clknode_methods[] = {
	/* Device interface */
	CLKNODEMETHOD(clknode_init,		aw_pll_init),
	CLKNODEMETHOD(clknode_set_gate,		aw_pll_set_gate),
	CLKNODEMETHOD(clknode_recalc_freq,	aw_pll_recalc),
	CLKNODEMETHOD(clknode_set_freq,		aw_pll_set_freq),
	CLKNODEMETHOD_END
};

DEFINE_CLASS_1(aw_pll_clknode, aw_pll_clknode_class, aw_pll_clknode_methods,
    sizeof(struct aw_pll_sc), clknode_class);

static int
aw_pll_create(device_t dev, bus_addr_t paddr, struct clkdom *clkdom,
    const char *pclkname, const char *clkname, int index)
{
	enum aw_pll_type type;
	struct clknode_init_def clkdef;
	struct aw_pll_sc *sc;
	struct clknode *clk;
	int error;

	type = ofw_bus_search_compatible(dev, compat_data)->ocd_data;

	memset(&clkdef, 0, sizeof(clkdef));
	clkdef.id = index;
	clkdef.name = clkname;
	if (pclkname != NULL) {
		clkdef.parent_names = malloc(sizeof(char *), M_OFWPROP,
		    M_WAITOK);
		clkdef.parent_names[0] = pclkname;
		clkdef.parent_cnt = 1;
	} else
		clkdef.parent_cnt = 0;

	if (aw_pll_func[type].init != NULL) {
		error = aw_pll_func[type].init(device_get_parent(dev),
		    paddr, &clkdef);
		if (error != 0) {
			device_printf(dev, "clock %s init failed\n", clkname);
			return (error);
		}
	}

	clk = clknode_create(clkdom, &aw_pll_clknode_class, &clkdef);
	if (clk == NULL) {
		device_printf(dev, "cannot create clock node\n");
		return (ENXIO);
	}
	sc = clknode_get_softc(clk);
	sc->clkdev = device_get_parent(dev);
	sc->reg = paddr;
	sc->type = type;
	sc->id = clkdef.id;

	clknode_register(clkdom, clk);

	OF_prop_free(__DECONST(char *, clkdef.parent_names));

	return (0);
}

static int
aw_pll_probe(device_t dev)
{
	if (!ofw_bus_status_okay(dev))
		return (ENXIO);

	if (ofw_bus_search_compatible(dev, compat_data)->ocd_data == 0)
		return (ENXIO);

	device_set_desc(dev, "Allwinner PLL Clock");
	return (BUS_PROBE_DEFAULT);
}

static int
aw_pll_attach(device_t dev)
{
	struct clkdom *clkdom;
	const char **names;
	int index, nout, error;
	clk_t clk_parent;
	uint32_t *indices;
	bus_addr_t paddr;
	bus_size_t psize;
	phandle_t node;

	node = ofw_bus_get_node(dev);

	if (ofw_reg_to_paddr(node, 0, &paddr, &psize, NULL) != 0) {
		device_printf(dev, "couldn't parse 'reg' property\n");
		return (ENXIO);
	}

	clkdom = clkdom_create(dev);

	nout = clk_parse_ofw_out_names(dev, node, &names, &indices);
	if (nout == 0) {
		device_printf(dev, "no clock outputs found\n");
		error = ENOENT;
		goto fail;
	}

	if (clk_get_by_ofw_index(dev, 0, &clk_parent) != 0)
		clk_parent = NULL;

	for (index = 0; index < nout; index++) {
		error = aw_pll_create(dev, paddr, clkdom,
		    clk_parent ? clk_get_name(clk_parent) : NULL,
		    names[index], nout == 1 ? 1 : index);
		if (error)
			goto fail;
	}

	if (clkdom_finit(clkdom) != 0) {
		device_printf(dev, "cannot finalize clkdom initialization\n");
		error = ENXIO;
		goto fail;
	}

	if (bootverbose)
		clkdom_dump(clkdom);

	return (0);

fail:
	return (error);
}

static device_method_t aw_pll_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		aw_pll_probe),
	DEVMETHOD(device_attach,	aw_pll_attach),

	DEVMETHOD_END
};

static driver_t aw_pll_driver = {
	"aw_pll",
	aw_pll_methods,
	0,
};

static devclass_t aw_pll_devclass;

EARLY_DRIVER_MODULE(aw_pll, simplebus, aw_pll_driver,
    aw_pll_devclass, 0, 0, BUS_PASS_BUS + BUS_PASS_ORDER_MIDDLE);
