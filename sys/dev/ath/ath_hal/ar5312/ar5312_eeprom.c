/*
 * Copyright (c) 2002-2008 Sam Leffler, Errno Consulting
 * Copyright (c) 2002-2008 Atheros Communications, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
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
 *
 * $FreeBSD: releng/11.0/sys/dev/ath/ath_hal/ar5312/ar5312_eeprom.c 204644 2010-03-03 17:32:32Z rpaulo $
 */
#include "opt_ah.h"


#ifdef AH_SUPPORT_AR5312

#include "ah.h"
#include "ah_internal.h"

#include "ar5312/ar5312.h"
#include "ar5312/ar5312reg.h"
#include "ar5212/ar5212desc.h"

/*
 * Read 16 bits of data from offset into *data
 */
HAL_BOOL
ar5312EepromRead(struct ath_hal *ah, u_int off, uint16_t *dataIn)
{
        int i,offset;
	const char *eepromAddr = AR5312_RADIOCONFIG(ah);
	uint8_t *data;
	
	data = (uint8_t *) dataIn;
	for (i=0,offset=2*off; i<2; i++,offset++) {
		data[i] = eepromAddr[offset];
	}
        return AH_TRUE;
}
#endif /* AH_SUPPORT_AR5312 */
