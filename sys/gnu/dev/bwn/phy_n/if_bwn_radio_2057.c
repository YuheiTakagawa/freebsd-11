
/*

  Broadcom B43 wireless driver
  IEEE 802.11n PHY data tables

  Copyright (c) 2008 Michael Buesch <m@bues.ch>
  Copyright (c) 2010 Rafał Miłecki <zajec5@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: releng/11.0/sys/gnu/dev/bwn/phy_n/if_bwn_radio_2057.c 300193 2016-05-19 05:06:12Z adrian $");

/*
 * The Broadcom Wireless LAN controller driver.
 */

#include "opt_wlan.h"
#include "opt_bwn.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/endian.h>
#include <sys/errno.h>
#include <sys/firmware.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/bus.h>
#include <sys/rman.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/if_llc.h>
#include <net/if_media.h>
#include <net/if_types.h>

#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/siba/siba_ids.h>
#include <dev/siba/sibareg.h>
#include <dev/siba/sibavar.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>
#include <net80211/ieee80211_regdomain.h>
#include <net80211/ieee80211_phy.h>
#include <net80211/ieee80211_ratectl.h>

#include <dev/bwn/if_bwnreg.h>
#include <dev/bwn/if_bwnvar.h>
#include <dev/bwn/if_bwn_debug.h>

#include <gnu/dev/bwn/phy_n/if_bwn_phy_n_regs.h>
#include <gnu/dev/bwn/phy_n/if_bwn_phy_n_ppr.h>
#include <gnu/dev/bwn/phy_n/if_bwn_phy_n_tables.h>
#include <gnu/dev/bwn/phy_n/if_bwn_phy_n_core.h>
#include <gnu/dev/bwn/phy_n/if_bwn_radio_2057.h>

static uint16_t r2057_rev4_init[][2] = {
	{ 0x0E, 0x20 }, { 0x31, 0x00 }, { 0x32, 0x00 }, { 0x33, 0x00 },
	{ 0x35, 0x26 }, { 0x3C, 0xff }, { 0x3D, 0xff }, { 0x3E, 0xff },
	{ 0x3F, 0xff }, { 0x62, 0x33 }, { 0x8A, 0xf0 }, { 0x8B, 0x10 },
	{ 0x8C, 0xf0 }, { 0x91, 0x3f }, { 0x92, 0x36 }, { 0xA4, 0x8c },
	{ 0xA8, 0x55 }, { 0xAF, 0x01 }, { 0x10F, 0xf0 }, { 0x110, 0x10 },
	{ 0x111, 0xf0 }, { 0x116, 0x3f }, { 0x117, 0x36 }, { 0x129, 0x8c },
	{ 0x12D, 0x55 }, { 0x134, 0x01 }, { 0x15E, 0x00 }, { 0x15F, 0x00 },
	{ 0x160, 0x00 }, { 0x161, 0x00 }, { 0x162, 0x00 }, { 0x163, 0x00 },
	{ 0x169, 0x02 }, { 0x16A, 0x00 }, { 0x16B, 0x00 }, { 0x16C, 0x00 },
	{ 0x1A4, 0x00 }, { 0x1A5, 0x00 }, { 0x1A6, 0x00 }, { 0x1AA, 0x00 },
	{ 0x1AB, 0x00 }, { 0x1AC, 0x00 },
};

static uint16_t r2057_rev5_init[][2] = {
	{ 0x00, 0x00 }, { 0x01, 0x57 }, { 0x02, 0x20 }, { 0x23, 0x6 },
	{ 0x31, 0x00 }, { 0x32, 0x00 }, { 0x33, 0x00 }, { 0x51, 0x70 },
	{ 0x59, 0x88 }, { 0x5C, 0x20 }, { 0x62, 0x33 }, { 0x63, 0x0f },
	{ 0x64, 0x0f }, { 0x81, 0x01 }, { 0x91, 0x3f }, { 0x92, 0x36 },
	{ 0xA1, 0x20 }, { 0xD6, 0x70 }, { 0xDE, 0x88 }, { 0xE1, 0x20 },
	{ 0xE8, 0x0f }, { 0xE9, 0x0f }, { 0x106, 0x01 }, { 0x116, 0x3f },
	{ 0x117, 0x36 }, { 0x126, 0x20 }, { 0x15E, 0x00 }, { 0x15F, 0x00 },
	{ 0x160, 0x00 }, { 0x161, 0x00 }, { 0x162, 0x00 }, { 0x163, 0x00 },
	{ 0x16A, 0x00 }, { 0x16B, 0x00 }, { 0x16C, 0x00 }, { 0x1A4, 0x00 },
	{ 0x1A5, 0x00 }, { 0x1A6, 0x00 }, { 0x1AA, 0x00 }, { 0x1AB, 0x00 },
	{ 0x1AC, 0x00 }, { 0x1B7, 0x0c }, { 0x1C1, 0x01 }, { 0x1C2, 0x80 },
};

static uint16_t r2057_rev5a_init[][2] = {
	{ 0x00, 0x15 }, { 0x01, 0x57 }, { 0x02, 0x20 }, { 0x23, 0x6 },
	{ 0x31, 0x00 }, { 0x32, 0x00 }, { 0x33, 0x00 }, { 0x51, 0x70 },
	{ 0x59, 0x88 }, { 0x5C, 0x20 }, { 0x62, 0x33 }, { 0x63, 0x0f },
	{ 0x64, 0x0f }, { 0x81, 0x01 }, { 0x91, 0x3f }, { 0x92, 0x36 },
	{ 0xC9, 0x01 }, { 0xD6, 0x70 }, { 0xDE, 0x88 }, { 0xE1, 0x20 },
	{ 0xE8, 0x0f }, { 0xE9, 0x0f }, { 0x106, 0x01 }, { 0x116, 0x3f },
	{ 0x117, 0x36 }, { 0x126, 0x20 }, { 0x14E, 0x01 }, { 0x15E, 0x00 },
	{ 0x15F, 0x00 }, { 0x160, 0x00 }, { 0x161, 0x00 }, { 0x162, 0x00 },
	{ 0x163, 0x00 }, { 0x16A, 0x00 }, { 0x16B, 0x00 }, { 0x16C, 0x00 },
	{ 0x1A4, 0x00 }, { 0x1A5, 0x00 }, { 0x1A6, 0x00 }, { 0x1AA, 0x00 },
	{ 0x1AB, 0x00 }, { 0x1AC, 0x00 }, { 0x1B7, 0x0c }, { 0x1C1, 0x01 },
	{ 0x1C2, 0x80 },
};

static uint16_t r2057_rev7_init[][2] = {
	{ 0x00, 0x00 }, { 0x01, 0x57 }, { 0x02, 0x20 }, { 0x31, 0x00 },
	{ 0x32, 0x00 }, { 0x33, 0x00 }, { 0x51, 0x70 }, { 0x59, 0x88 },
	{ 0x5C, 0x20 }, { 0x62, 0x33 }, { 0x63, 0x0f }, { 0x64, 0x13 },
	{ 0x66, 0xee }, { 0x6E, 0x58 }, { 0x75, 0x13 }, { 0x7B, 0x13 },
	{ 0x7C, 0x14 }, { 0x7D, 0xee }, { 0x81, 0x01 }, { 0x91, 0x3f },
	{ 0x92, 0x36 }, { 0xA1, 0x20 }, { 0xD6, 0x70 }, { 0xDE, 0x88 },
	{ 0xE1, 0x20 }, { 0xE8, 0x0f }, { 0xE9, 0x13 }, { 0xEB, 0xee },
	{ 0xF3, 0x58 }, { 0xFA, 0x13 }, { 0x100, 0x13 }, { 0x101, 0x14 },
	{ 0x102, 0xee }, { 0x106, 0x01 }, { 0x116, 0x3f }, { 0x117, 0x36 },
	{ 0x126, 0x20 }, { 0x15E, 0x00 }, { 0x15F, 0x00 }, { 0x160, 0x00 },
	{ 0x161, 0x00 }, { 0x162, 0x00 }, { 0x163, 0x00 }, { 0x16A, 0x00 },
	{ 0x16B, 0x00 }, { 0x16C, 0x00 }, { 0x1A4, 0x00 }, { 0x1A5, 0x00 },
	{ 0x1A6, 0x00 }, { 0x1AA, 0x00 }, { 0x1AB, 0x00 }, { 0x1AC, 0x00 },
	{ 0x1B7, 0x05 }, { 0x1C2, 0xa0 },
};

/* TODO: Which devices should use it?
static uint16_t r2057_rev8_init[][2] = {
	{ 0x00, 0x08 }, { 0x01, 0x57 }, { 0x02, 0x20 }, { 0x31, 0x00 },
	{ 0x32, 0x00 }, { 0x33, 0x00 }, { 0x51, 0x70 }, { 0x59, 0x88 },
	{ 0x5C, 0x20 }, { 0x62, 0x33 }, { 0x63, 0x0f }, { 0x64, 0x0f },
	{ 0x6E, 0x58 }, { 0x75, 0x13 }, { 0x7B, 0x13 }, { 0x7C, 0x0f },
	{ 0x7D, 0xee }, { 0x81, 0x01 }, { 0x91, 0x3f }, { 0x92, 0x36 },
	{ 0xA1, 0x20 }, { 0xC9, 0x01 }, { 0xD6, 0x70 }, { 0xDE, 0x88 },
	{ 0xE1, 0x20 }, { 0xE8, 0x0f }, { 0xE9, 0x0f }, { 0xF3, 0x58 },
	{ 0xFA, 0x13 }, { 0x100, 0x13 }, { 0x101, 0x0f }, { 0x102, 0xee },
	{ 0x106, 0x01 }, { 0x116, 0x3f }, { 0x117, 0x36 }, { 0x126, 0x20 },
	{ 0x14E, 0x01 }, { 0x15E, 0x00 }, { 0x15F, 0x00 }, { 0x160, 0x00 },
	{ 0x161, 0x00 }, { 0x162, 0x00 }, { 0x163, 0x00 }, { 0x16A, 0x00 },
	{ 0x16B, 0x00 }, { 0x16C, 0x00 }, { 0x1A4, 0x00 }, { 0x1A5, 0x00 },
	{ 0x1A6, 0x00 }, { 0x1AA, 0x00 }, { 0x1AB, 0x00 }, { 0x1AC, 0x00 },
	{ 0x1B7, 0x05 }, { 0x1C2, 0xa0 },
};
*/

/* Extracted from MMIO dump of 6.30.223.141 */
static uint16_t r2057_rev9_init[][2] = {
	{ 0x27, 0x1f }, { 0x28, 0x0a }, { 0x29, 0x2f }, { 0x42, 0x1f },
	{ 0x48, 0x3f }, { 0x5c, 0x41 }, { 0x63, 0x14 }, { 0x64, 0x12 },
	{ 0x66, 0xff }, { 0x74, 0xa3 }, { 0x7b, 0x14 }, { 0x7c, 0x14 },
	{ 0x7d, 0xee }, { 0x86, 0xc0 }, { 0xc4, 0x10 }, { 0xc9, 0x01 },
	{ 0xe1, 0x41 }, { 0xe8, 0x14 }, { 0xe9, 0x12 }, { 0xeb, 0xff },
	{ 0xf5, 0x0a }, { 0xf8, 0x09 }, { 0xf9, 0xa3 }, { 0x100, 0x14 },
	{ 0x101, 0x10 }, { 0x102, 0xee }, { 0x10b, 0xc0 }, { 0x149, 0x10 },
	{ 0x14e, 0x01 }, { 0x1b7, 0x05 }, { 0x1c2, 0xa0 },
};

/* Extracted from MMIO dump of 6.30.223.248 */
static uint16_t r2057_rev14_init[][2] = {
	{ 0x011, 0xfc }, { 0x030, 0x24 }, { 0x040, 0x1c }, { 0x082, 0x08 },
	{ 0x0b4, 0x44 }, { 0x0c8, 0x01 }, { 0x0c9, 0x01 }, { 0x107, 0x08 },
	{ 0x14d, 0x01 }, { 0x14e, 0x01 }, { 0x1af, 0x40 }, { 0x1b0, 0x40 },
	{ 0x1cc, 0x01 }, { 0x1cf, 0x10 }, { 0x1d0, 0x0f }, { 0x1d3, 0x10 },
	{ 0x1d4, 0x0f },
};

#define RADIOREGS7(r00, r01, r02, r03, r04, r05, r06, r07, r08, r09, \
		   r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, \
		   r20, r21, r22, r23, r24, r25, r26, r27) \
	.radio_vcocal_countval0			= r00,	\
	.radio_vcocal_countval1			= r01,	\
	.radio_rfpll_refmaster_sparextalsize	= r02,	\
	.radio_rfpll_loopfilter_r1		= r03,	\
	.radio_rfpll_loopfilter_c2		= r04,	\
	.radio_rfpll_loopfilter_c1		= r05,	\
	.radio_cp_kpd_idac			= r06,	\
	.radio_rfpll_mmd0			= r07,	\
	.radio_rfpll_mmd1			= r08,	\
	.radio_vcobuf_tune			= r09,	\
	.radio_logen_mx2g_tune			= r10,	\
	.radio_logen_mx5g_tune			= r11,	\
	.radio_logen_indbuf2g_tune		= r12,	\
	.radio_logen_indbuf5g_tune		= r13,	\
	.radio_txmix2g_tune_boost_pu_core0	= r14,	\
	.radio_pad2g_tune_pus_core0		= r15,	\
	.radio_pga_boost_tune_core0		= r16,	\
	.radio_txmix5g_boost_tune_core0		= r17,	\
	.radio_pad5g_tune_misc_pus_core0	= r18,	\
	.radio_lna2g_tune_core0			= r19,	\
	.radio_lna5g_tune_core0			= r20,	\
	.radio_txmix2g_tune_boost_pu_core1	= r21,	\
	.radio_pad2g_tune_pus_core1		= r22,	\
	.radio_pga_boost_tune_core1		= r23,	\
	.radio_txmix5g_boost_tune_core1		= r24,	\
	.radio_pad5g_tune_misc_pus_core1	= r25,	\
	.radio_lna2g_tune_core1			= r26,	\
	.radio_lna5g_tune_core1			= r27

#define RADIOREGS7_2G(r00, r01, r02, r03, r04, r05, r06, r07, r08, r09, \
		      r10, r11, r12, r13, r14, r15, r16, r17) \
	.radio_vcocal_countval0			= r00,	\
	.radio_vcocal_countval1			= r01,	\
	.radio_rfpll_refmaster_sparextalsize	= r02,	\
	.radio_rfpll_loopfilter_r1		= r03,	\
	.radio_rfpll_loopfilter_c2		= r04,	\
	.radio_rfpll_loopfilter_c1		= r05,	\
	.radio_cp_kpd_idac			= r06,	\
	.radio_rfpll_mmd0			= r07,	\
	.radio_rfpll_mmd1			= r08,	\
	.radio_vcobuf_tune			= r09,	\
	.radio_logen_mx2g_tune			= r10,	\
	.radio_logen_indbuf2g_tune		= r11,	\
	.radio_txmix2g_tune_boost_pu_core0	= r12,	\
	.radio_pad2g_tune_pus_core0		= r13,	\
	.radio_lna2g_tune_core0			= r14,	\
	.radio_txmix2g_tune_boost_pu_core1	= r15,	\
	.radio_pad2g_tune_pus_core1		= r16,	\
	.radio_lna2g_tune_core1			= r17

#define PHYREGS(r0, r1, r2, r3, r4, r5)	\
	.phy_regs.phy_bw1a	= r0,	\
	.phy_regs.phy_bw2	= r1,	\
	.phy_regs.phy_bw3	= r2,	\
	.phy_regs.phy_bw4	= r3,	\
	.phy_regs.phy_bw5	= r4,	\
	.phy_regs.phy_bw6	= r5

/* Copied from brcmsmac (5.75.11): chan_info_nphyrev8_2057_rev5 */
static const struct bwn_nphy_chantabent_rev7_2g bwn_nphy_chantab_phy_rev8_radio_rev5[] = {
	{
		.freq			= 2412,
		RADIOREGS7_2G(0x48, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x6c,
			      0x09, 0x0d, 0x08, 0x0e, 0x61, 0x03, 0xff, 0x61,
			      0x03, 0xff),
		PHYREGS(0x03c9, 0x03c5, 0x03c1, 0x043a, 0x043f, 0x0443),
	},
	{
		.freq			= 2417,
		RADIOREGS7_2G(0x4b, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x71,
			      0x09, 0x0d, 0x08, 0x0e, 0x61, 0x03, 0xff, 0x61,
			      0x03, 0xff),
		PHYREGS(0x03cb, 0x03c7, 0x03c3, 0x0438, 0x043d, 0x0441),
	},
	{
		.freq			= 2422,
		RADIOREGS7_2G(0x4e, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x76,
			      0x09, 0x0d, 0x08, 0x0e, 0x61, 0x03, 0xef, 0x61,
			      0x03, 0xef),
		PHYREGS(0x03cd, 0x03c9, 0x03c5, 0x0436, 0x043a, 0x043f),
	},
	{
		.freq			= 2427,
		RADIOREGS7_2G(0x52, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x7b,
			      0x09, 0x0c, 0x08, 0x0e, 0x61, 0x03, 0xdf, 0x61,
			      0x03, 0xdf),
		PHYREGS(0x03cf, 0x03cb, 0x03c7, 0x0434, 0x0438, 0x043d),
	},
	{
		.freq			= 2432,
		RADIOREGS7_2G(0x55, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x80,
			      0x09, 0x0c, 0x07, 0x0d, 0x61, 0x03, 0xcf, 0x61,
			      0x03, 0xcf),
		PHYREGS(0x03d1, 0x03cd, 0x03c9, 0x0431, 0x0436, 0x043a),
	},
	{
		.freq			= 2437,
		RADIOREGS7_2G(0x58, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x85,
			      0x09, 0x0c, 0x07, 0x0d, 0x61, 0x03, 0xbf, 0x61,
			      0x03, 0xbf),
		PHYREGS(0x03d3, 0x03cf, 0x03cb, 0x042f, 0x0434, 0x0438),
	},
	{
		.freq			= 2442,
		RADIOREGS7_2G(0x5c, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x8a,
			      0x09, 0x0b, 0x07, 0x0d, 0x61, 0x03, 0xaf, 0x61,
			      0x03, 0xaf),
		PHYREGS(0x03d5, 0x03d1, 0x03cd, 0x042d, 0x0431, 0x0436),
	},
	{
		.freq			= 2447,
		RADIOREGS7_2G(0x5f, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x8f,
			      0x09, 0x0b, 0x07, 0x0d, 0x61, 0x03, 0x9f, 0x61,
			      0x03, 0x9f),
		PHYREGS(0x03d7, 0x03d3, 0x03cf, 0x042b, 0x042f, 0x0434),
	},
	{
		.freq			= 2452,
		RADIOREGS7_2G(0x62, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x94,
			      0x09, 0x0b, 0x07, 0x0d, 0x61, 0x03, 0x8f, 0x61,
			      0x03, 0x8f),
		PHYREGS(0x03d9, 0x03d5, 0x03d1, 0x0429, 0x042d, 0x0431),
	},
	{
		.freq			= 2457,
		RADIOREGS7_2G(0x66, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x99,
			      0x09, 0x0b, 0x07, 0x0c, 0x61, 0x03, 0x7f, 0x61,
			      0x03, 0x7f),
		PHYREGS(0x03db, 0x03d7, 0x03d3, 0x0427, 0x042b, 0x042f),
	},
	{
		.freq			= 2462,
		RADIOREGS7_2G(0x69, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x9e,
			      0x09, 0x0b, 0x07, 0x0c, 0x61, 0x03, 0x6f, 0x61,
			      0x03, 0x6f),
		PHYREGS(0x03dd, 0x03d9, 0x03d5, 0x0424, 0x0429, 0x042d),
	},
	{
		.freq			= 2467,
		RADIOREGS7_2G(0x6c, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0xa3,
			      0x09, 0x0b, 0x06, 0x0c, 0x61, 0x03, 0x5f, 0x61,
			      0x03, 0x5f),
		PHYREGS(0x03df, 0x03db, 0x03d7, 0x0422, 0x0427, 0x042b),
	},
	{
		.freq			= 2472,
		RADIOREGS7_2G(0x70, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0xa8,
			      0x09, 0x0a, 0x06, 0x0b, 0x61, 0x03, 0x4f, 0x61,
			      0x03, 0x4f),
		PHYREGS(0x03e1, 0x03dd, 0x03d9, 0x0420, 0x0424, 0x0429),
	},
	{
		.freq			= 2484,
		RADIOREGS7_2G(0x78, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0xb4,
			      0x09, 0x0a, 0x06, 0x0b, 0x61, 0x03, 0x3f, 0x61,
			      0x03, 0x3f),
		PHYREGS(0x03e6, 0x03e2, 0x03de, 0x041b, 0x041f, 0x0424),
	}
};

/* Extracted from MMIO dump of 6.30.223.248 */
static const struct bwn_nphy_chantabent_rev7_2g bwn_nphy_chantab_phy_rev17_radio_rev14[] = {
	{
		.freq			= 2412,
		RADIOREGS7_2G(0x48, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x6c,
			      0x09, 0x0d, 0x09, 0x03, 0x21, 0x53, 0xff, 0x21,
			      0x53, 0xff),
		PHYREGS(0x03c9, 0x03c5, 0x03c1, 0x043a, 0x043f, 0x0443),
	},
	{
		.freq			= 2417,
		RADIOREGS7_2G(0x4b, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x71,
			      0x09, 0x0d, 0x08, 0x03, 0x21, 0x53, 0xff, 0x21,
			      0x53, 0xff),
		PHYREGS(0x03cb, 0x03c7, 0x03c3, 0x0438, 0x043d, 0x0441),
	},
	{
		.freq			= 2422,
		RADIOREGS7_2G(0x4e, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x76,
			      0x09, 0x0d, 0x08, 0x03, 0x21, 0x53, 0xff, 0x21,
			      0x53, 0xff),
		PHYREGS(0x03cd, 0x03c9, 0x03c5, 0x0436, 0x043a, 0x043f),
	},
	{
		.freq			= 2427,
		RADIOREGS7_2G(0x52, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x7b,
			      0x09, 0x0c, 0x08, 0x03, 0x21, 0x53, 0xff, 0x21,
			      0x53, 0xff),
		PHYREGS(0x03cf, 0x03cb, 0x03c7, 0x0434, 0x0438, 0x043d),
	},
	{
		.freq			= 2432,
		RADIOREGS7_2G(0x55, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x80,
			      0x09, 0x0c, 0x08, 0x03, 0x21, 0x53, 0xff, 0x21,
			      0x53, 0xff),
		PHYREGS(0x03d1, 0x03cd, 0x03c9, 0x0431, 0x0436, 0x043a),
	},
	{
		.freq			= 2437,
		RADIOREGS7_2G(0x58, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x85,
			      0x09, 0x0c, 0x08, 0x03, 0x21, 0x53, 0xff, 0x21,
			      0x53, 0xff),
		PHYREGS(0x03d3, 0x03cf, 0x03cb, 0x042f, 0x0434, 0x0438),
	},
	{
		.freq			= 2442,
		RADIOREGS7_2G(0x5c, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x8a,
			      0x09, 0x0c, 0x08, 0x03, 0x21, 0x43, 0xff, 0x21,
			      0x43, 0xff),
		PHYREGS(0x03d5, 0x03d1, 0x03cd, 0x042d, 0x0431, 0x0436),
	},
	{
		.freq			= 2447,
		RADIOREGS7_2G(0x5f, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x8f,
			      0x09, 0x0c, 0x08, 0x03, 0x21, 0x43, 0xff, 0x21,
			      0x43, 0xff),
		PHYREGS(0x03d7, 0x03d3, 0x03cf, 0x042b, 0x042f, 0x0434),
	},
	{
		.freq			= 2452,
		RADIOREGS7_2G(0x62, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x94,
			      0x09, 0x0c, 0x08, 0x03, 0x21, 0x43, 0xff, 0x21,
			      0x43, 0xff),
		PHYREGS(0x03d9, 0x03d5, 0x03d1, 0x0429, 0x042d, 0x0431),
	},
	{
		.freq			= 2457,
		RADIOREGS7_2G(0x66, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x99,
			      0x09, 0x0b, 0x07, 0x03, 0x21, 0x43, 0xff, 0x21,
			      0x43, 0xff),
		PHYREGS(0x03db, 0x03d7, 0x03d3, 0x0427, 0x042b, 0x042f),
	},
	{
		.freq			= 2462,
		RADIOREGS7_2G(0x69, 0x16, 0x30, 0x2b, 0x1f, 0x1f, 0x30, 0x9e,
			      0x09, 0x0b, 0x07, 0x03, 0x01, 0x43, 0xff, 0x01,
			      0x43, 0xff),
		PHYREGS(0x03dd, 0x03d9, 0x03d5, 0x0424, 0x0429, 0x042d),
	},
};

/* Extracted from MMIO dump of 6.30.223.141 */
static const struct bwn_nphy_chantabent_rev7 bwn_nphy_chantab_phy_rev16_radio_rev9[] = {
	{
		.freq			= 2412,
		RADIOREGS7(0x48, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x6c,
			   0x09, 0x0f, 0x0a, 0x00, 0x0a, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03c9, 0x03c5, 0x03c1, 0x043a, 0x043f, 0x0443),
	},
	{
		.freq			= 2417,
		RADIOREGS7(0x4b, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x71,
			   0x09, 0x0f, 0x0a, 0x00, 0x0a, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03cb, 0x03c7, 0x03c3, 0x0438, 0x043d, 0x0441),
	},
	{
		.freq			= 2422,
		RADIOREGS7(0x4e, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x76,
			   0x09, 0x0f, 0x09, 0x00, 0x09, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03cd, 0x03c9, 0x03c5, 0x0436, 0x043a, 0x043f),
	},
	{
		.freq			= 2427,
		RADIOREGS7(0x52, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x7b,
			   0x09, 0x0f, 0x09, 0x00, 0x09, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03cf, 0x03cb, 0x03c7, 0x0434, 0x0438, 0x043d),
	},
	{
		.freq			= 2432,
		RADIOREGS7(0x55, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x80,
			   0x09, 0x0f, 0x08, 0x00, 0x08, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03d1, 0x03cd, 0x03c9, 0x0431, 0x0436, 0x043a),
	},
	{
		.freq			= 2437,
		RADIOREGS7(0x58, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x85,
			   0x09, 0x0f, 0x08, 0x00, 0x08, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03d3, 0x03cf, 0x03cb, 0x042f, 0x0434, 0x0438),
	},
	{
		.freq			= 2442,
		RADIOREGS7(0x5c, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x8a,
			   0x09, 0x0f, 0x07, 0x00, 0x07, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03d5, 0x03d1, 0x03cd, 0x042d, 0x0431, 0x0436),
	},
	{
		.freq			= 2447,
		RADIOREGS7(0x5f, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x8f,
			   0x09, 0x0f, 0x07, 0x00, 0x07, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03d7, 0x03d3, 0x03cf, 0x042b, 0x042f, 0x0434),
	},
	{
		.freq			= 2452,
		RADIOREGS7(0x62, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x94,
			   0x09, 0x0f, 0x07, 0x00, 0x07, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03d9, 0x03d5, 0x03d1, 0x0429, 0x042d, 0x0431),
	},
	{
		.freq			= 2457,
		RADIOREGS7(0x66, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x99,
			   0x09, 0x0f, 0x06, 0x00, 0x06, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03db, 0x03d7, 0x03d3, 0x0427, 0x042b, 0x042f),
	},
	{
		.freq			= 2462,
		RADIOREGS7(0x69, 0x16, 0x30, 0x1b, 0x0a, 0x0a, 0x30, 0x9e,
			   0x09, 0x0f, 0x06, 0x00, 0x06, 0x00, 0x41, 0x63,
			   0x00, 0x00, 0x00, 0xf0, 0x00, 0x41, 0x63, 0x00,
			   0x00, 0x00, 0xf0, 0x00),
		PHYREGS(0x03dd, 0x03d9, 0x03d5, 0x0424, 0x0429, 0x042d),
	},
	{
		.freq			= 5180,
		RADIOREGS7(0xbe, 0x16, 0x10, 0x1f, 0x08, 0x08, 0x3f, 0x06,
			   0x02, 0x0e, 0x00, 0x0e, 0x00, 0x9e, 0x00, 0x00,
			   0x9f, 0x2f, 0xa3, 0x00, 0xfc, 0x00, 0x00, 0x4f,
			   0x3a, 0x83, 0x00, 0xfc),
		PHYREGS(0x081c, 0x0818, 0x0814, 0x01f9, 0x01fa, 0x01fb),
	},
	{
		.freq			= 5200,
		RADIOREGS7(0xc5, 0x16, 0x10, 0x1f, 0x08, 0x08, 0x3f, 0x08,
			   0x02, 0x0e, 0x00, 0x0e, 0x00, 0x9e, 0x00, 0x00,
			   0x7f, 0x2f, 0x83, 0x00, 0xf8, 0x00, 0x00, 0x4c,
			   0x4a, 0x83, 0x00, 0xf8),
		PHYREGS(0x0824, 0x0820, 0x081c, 0x01f7, 0x01f8, 0x01f9),
	},
	{
		.freq			= 5220,
		RADIOREGS7(0xcc, 0x16, 0x10, 0x1f, 0x08, 0x08, 0x3f, 0x0a,
			   0x02, 0x0e, 0x00, 0x0e, 0x00, 0x9e, 0x00, 0x00,
			   0x6d, 0x3d, 0x83, 0x00, 0xf8, 0x00, 0x00, 0x2d,
			   0x2a, 0x73, 0x00, 0xf8),
		PHYREGS(0x082c, 0x0828, 0x0824, 0x01f5, 0x01f6, 0x01f7),
	},
	{
		.freq			= 5240,
		RADIOREGS7(0xd2, 0x16, 0x10, 0x1f, 0x08, 0x08, 0x3f, 0x0c,
			   0x02, 0x0d, 0x00, 0x0d, 0x00, 0x8d, 0x00, 0x00,
			   0x4d, 0x1c, 0x73, 0x00, 0xf8, 0x00, 0x00, 0x4d,
			   0x2b, 0x73, 0x00, 0xf8),
		PHYREGS(0x0834, 0x0830, 0x082c, 0x01f3, 0x01f4, 0x01f5),
	},
	{
		.freq			= 5745,
		RADIOREGS7(0x7b, 0x17, 0x20, 0x1f, 0x08, 0x08, 0x3f, 0x7d,
			   0x04, 0x08, 0x00, 0x06, 0x00, 0x15, 0x00, 0x00,
			   0x08, 0x03, 0x03, 0x00, 0x30, 0x00, 0x00, 0x06,
			   0x02, 0x03, 0x00, 0x30),
		PHYREGS(0x08fe, 0x08fa, 0x08f6, 0x01c8, 0x01c8, 0x01c9),
	},
	{
		.freq			= 5765,
		RADIOREGS7(0x81, 0x17, 0x20, 0x1f, 0x08, 0x08, 0x3f, 0x81,
			   0x04, 0x08, 0x00, 0x06, 0x00, 0x15, 0x00, 0x00,
			   0x06, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x05,
			   0x02, 0x03, 0x00, 0x00),
		PHYREGS(0x0906, 0x0902, 0x08fe, 0x01c6, 0x01c7, 0x01c8),
	},
	{
		.freq			= 5785,
		RADIOREGS7(0x88, 0x17, 0x20, 0x1f, 0x08, 0x08, 0x3f, 0x85,
			   0x04, 0x08, 0x00, 0x06, 0x00, 0x15, 0x00, 0x00,
			   0x08, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x05,
			   0x21, 0x03, 0x00, 0x00),
		PHYREGS(0x090e, 0x090a, 0x0906, 0x01c4, 0x01c5, 0x01c6),
	},
	{
		.freq			= 5805,
		RADIOREGS7(0x8f, 0x17, 0x20, 0x1f, 0x08, 0x08, 0x3f, 0x89,
			   0x04, 0x07, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00,
			   0x06, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
			   0x00, 0x03, 0x00, 0x00),
		PHYREGS(0x0916, 0x0912, 0x090e, 0x01c3, 0x01c4, 0x01c4),
	},
	{
		.freq			= 5825,
		RADIOREGS7(0x95, 0x17, 0x20, 0x1f, 0x08, 0x08, 0x3f, 0x8d,
			   0x04, 0x07, 0x00, 0x05, 0x00, 0x03, 0x00, 0x00,
			   0x05, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
			   0x00, 0x03, 0x00, 0x00),
		PHYREGS(0x091e, 0x091a, 0x0916, 0x01c1, 0x01c2, 0x01c3),
	},
};

void r2057_upload_inittabs(struct bwn_mac *mac)
{
	struct bwn_phy *phy = &mac->mac_phy;
	uint16_t *table = NULL;
	uint16_t size, i;

	switch (phy->rev) {
	case 7:
		table = r2057_rev4_init[0];
		size = nitems(r2057_rev4_init);
		break;
	case 8:
		if (phy->rf_rev == 5) {
			table = r2057_rev5_init[0];
			size = nitems(r2057_rev5_init);
		} else if (phy->rf_rev == 7) {
			table = r2057_rev7_init[0];
			size = nitems(r2057_rev7_init);
		}
		break;
	case 9:
		if (phy->rf_rev == 5) {
			table = r2057_rev5a_init[0];
			size = nitems(r2057_rev5a_init);
		}
		break;
	case 16:
		if (phy->rf_rev == 9) {
			table = r2057_rev9_init[0];
			size = nitems(r2057_rev9_init);
		}
		break;
	case 17:
		if (phy->rf_rev == 14) {
			table = r2057_rev14_init[0];
			size = nitems(r2057_rev14_init);
		}
		break;
	}

	if (! table) {
		device_printf(mac->mac_sc->sc_dev,
		    "%s: couldn't find a suitable table (phy ref=%d, rf_ref=%d)\n",
		    __func__,
		    phy->rev,
		    phy->rf_rev);
	}

	if (table) {
		for (i = 0; i < size; i++, table += 2)
			BWN_RF_WRITE(mac, table[0], table[1]);
	}
}

void r2057_get_chantabent_rev7(struct bwn_mac *mac, uint16_t freq,
			       const struct bwn_nphy_chantabent_rev7 **tabent_r7,
			       const struct bwn_nphy_chantabent_rev7_2g **tabent_r7_2g)
{
	struct bwn_phy *phy = &mac->mac_phy;
	const struct bwn_nphy_chantabent_rev7 *e_r7 = NULL;
	const struct bwn_nphy_chantabent_rev7_2g *e_r7_2g = NULL;
	unsigned int len, i;

	*tabent_r7 = NULL;
	*tabent_r7_2g = NULL;

	switch (phy->rev) {
	case 8:
		if (phy->rf_rev == 5) {
			e_r7_2g = bwn_nphy_chantab_phy_rev8_radio_rev5;
			len = nitems(bwn_nphy_chantab_phy_rev8_radio_rev5);
		}
		break;
	case 16:
		if (phy->rf_rev == 9) {
			e_r7 = bwn_nphy_chantab_phy_rev16_radio_rev9;
			len = nitems(bwn_nphy_chantab_phy_rev16_radio_rev9);
		}
		break;
	case 17:
		if (phy->rf_rev == 14) {
			e_r7_2g = bwn_nphy_chantab_phy_rev17_radio_rev14;
			len = nitems(bwn_nphy_chantab_phy_rev17_radio_rev14);
		}
		break;
	default:
		break;
	}

	if (e_r7) {
		for (i = 0; i < len; i++, e_r7++) {
			if (e_r7->freq == freq) {
				*tabent_r7 = e_r7;
				return;
			}
		}
	} else if (e_r7_2g) {
		for (i = 0; i < len; i++, e_r7_2g++) {
			if (e_r7_2g->freq == freq) {
				*tabent_r7_2g = e_r7_2g;
				return;
			}
		}
	} else {
		device_printf(mac->mac_sc->sc_dev,
		    "%s: couldn't find a suitable chantab\n",
		    __func__);
	}
}
