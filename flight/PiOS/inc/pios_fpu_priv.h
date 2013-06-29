/**
 ******************************************************************************
 * @file       pios_fpu_priv.h
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

#if !defined(PIOS_FPU_PRIV_H)
#define PIOS_FPU_PRIV_H

extern const struct pios_com_driver pios_can_com_driver;

struct pios_fpu_cfg {
	
	struct stm32_irq irq;    //! Configuration for IRQ
};

/**
 * Initialize the FPU driver and return an opaque id
 * @param[out]   id the FPU interface handle
 * @param[in]    cfg the configuration structure
 * @return 0 if successful, negative otherwise
 */
int32_t PIOS_FPU_Init(uintptr_t *id, const struct pios_fpu_cfg *cfg);

void PIOS_FPU_Handler();

#endif /* PIOS_FPU_PRIV_H */

/**
 * @}
 * @}
 */
 
