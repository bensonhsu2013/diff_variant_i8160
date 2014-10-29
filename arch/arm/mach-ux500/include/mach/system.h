/*
 * Copyright (C) 2009 ST-Ericsson.
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <mach/prcmu.h>
#include <mach/sec_common.h>
#include <mach/reboot_reasons.h>
#if defined(CONFIG_MACH_CODINA_CHN)
#include <linux/rtc.h>
extern struct rtc_wkalrm autoboot_alm;
#endif

static inline void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
#ifdef CONFIG_UX500_SOC_DB8500
	unsigned short reason ;
	unsigned char regValue;

	reason= sec_common_update_reboot_reason( mode, cmd ) ;

	#if defined(CONFIG_MACH_CODINA_CHN)
	if ((mode == 'L' || mode == 'U') || (autoboot_alm.enabled == 1))
	#else
	if ((mode == 'L' || mode == 'U'))
	#endif
	{
		/* Call the PRCMU reset API (w/o reset reason code) */
		prcmu_system_reset( reason );
	} else {
		/* reason[8 LSB] */
		regValue = reason & 0xFF;
		prcmu_abb_write(0xF, 0x8, &regValue, 1);

		/* reason[8 MSB] */
		regValue = (reason>>8) & 0xFF;
		prcmu_abb_write(0xF, 0x9, &regValue, 1);

		/* timeout 0s */
		regValue = 0x0; /* timeout 0s */
		prcmu_abb_write(0x2, 0x2, &regValue, 1);

		/* AB watchdog enable & reboot */
		regValue = 0x13;
		prcmu_abb_write(0x2, 0x1, &regValue, 1);
	}
#endif
}

#endif
