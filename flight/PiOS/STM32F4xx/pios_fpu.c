/**
 ******************************************************************************
 * @file       pios_fpu.c
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
 * @addtogroup [Group]
 * @{
 * @addtogroup %CLASS%
 * @{
 * @brief [Brief]
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* Project Includes */
#include "pios.h"
#include "pios_fpu_priv.h"

#if defined(PIOS_INCLUDE_FPU)

/* Local Variables */

enum pios_fpu_dev_magic {
	PIOS_FPU_DEV_MAGIC = 0x74872484,
};

struct pios_fpu_dev {
	enum pios_fpu_dev_magic magic;
	const struct pios_fpu_cfg *cfg;
	uint32_t execption_count_ioc;
	uint32_t execption_count_dzc;
	uint32_t execption_count_ofc;
	uint32_t execption_count_ufc;
	uint32_t execption_count_ixc;
	uint32_t execption_count_idc;
};

static bool PIOS_FPU_validate(struct pios_fpu_dev *fpu_dev)
{
	return (fpu_dev->magic == PIOS_FPU_DEV_MAGIC);
}

//! The local handle for the FPU device
static struct pios_fpu_dev fpu_dev_singleton;
static struct pios_fpu_dev *fpu_dev;

static struct pios_fpu_dev* PIOS_FPU_alloc(void)
{
	if (fpu_dev != NULL) {
		return NULL;
	}

	//This is a singleton
	fpu_dev = &fpu_dev_singleton;

	memset(fpu_dev, 0, sizeof(fpu_dev));

	fpu_dev->magic = PIOS_FPU_DEV_MAGIC;

	return fpu_dev;
}


/**
 * Initialize the FPU driver and return an opaque id
 * @param[out]   id the FPU interface handle
 * @param[in]    cfg the configuration structure
 * @return 0 if successful, negative otherwise
 */
int32_t PIOS_FPU_Init(uintptr_t *fpu_id, const struct pios_fpu_cfg *cfg)
{
	PIOS_DEBUG_Assert(fpu_id);
	PIOS_DEBUG_Assert(cfg);

	fpu_dev = (struct pios_fpu_dev*)PIOS_FPU_alloc();
	if (fpu_dev == NULL)
		return -1;

	/* Bind the configuration to the device instance */
	fpu_dev->cfg = cfg;

	// Enable the IRQ
 	NVIC_Init((NVIC_InitTypeDef*)&fpu_dev->cfg->irq.init);

	return 0;
}

static void Handle_FPU_Exception(uint32_t lr, uint32_t sp)
{
	PIOS_Assert(PIOS_FPU_validate(fpu_dev));

	register uint32_t fpscr_val;
	if (lr == 0xFFFFFFE9)
	{
		sp = sp + 0x60;
	}
	else if (lr == 0xFFFFFFED)
	{
		sp = __get_PSP() + 0x60;
	}
	fpscr_val = *(uint32_t*)sp;
	{
		static const uint32_t IOC_FLAG = (1 << 0);  // invalid operation
		static const uint32_t DZC_FLAG = (1 << 1);  // division by zero
		static const uint32_t OFC_FLAG = (1 << 2);  // overflow
		static const uint32_t UFC_FLAG = (1 << 3);  // underflow
		static const uint32_t IXC_FLAG = (1 << 4);  // inexact
		static const uint32_t IDC_FLAG = (1 << 7);  // input denormal

		//check exception flags
		if ((fpscr_val & IOC_FLAG) != 0)
			++fpu_dev->execption_count_ioc;
		if ((fpscr_val & DZC_FLAG) != 0)
			++fpu_dev->execption_count_dzc;
		if ((fpscr_val & OFC_FLAG) != 0)
			++fpu_dev->execption_count_ofc;
		if ((fpscr_val & UFC_FLAG) != 0)
			++fpu_dev->execption_count_ufc;
		if ((fpscr_val & IXC_FLAG) != 0)
			++fpu_dev->execption_count_ixc;
		if ((fpscr_val & IDC_FLAG) != 0)
			++fpu_dev->execption_count_idc;

	}
	fpscr_val &= (uint32_t)~0x8F;
	// Clear all exception flags
	*(uint32_t*)sp = fpscr_val;
	__DMB();
}

__attribute__((naked)) void PIOS_FPU_Handler()
{
	asm("vmrs r2, fpscr");

	register uint32_t __regSP __ASM("sp");
	register uint32_t __regLR __ASM("lr");

	Handle_FPU_Exception(__regLR, __regSP);

	asm("bx lr");
}

#endif
