/***************************************************************************//**
 * @file
 * @brief Memory protection unit (MPU) peripheral API for EFM32.
 * @author Energy Micro AS
 * @version 2.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef __EFM32_MPU_H
#define __EFM32_MPU_H

#include "efm32.h"

#if defined(__MPU_PRESENT) && (__MPU_PRESENT == 1)
#include "efm32_assert.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup MPU
 * @{
 ******************************************************************************/

/** @anchor MPU_CTRL_PRIVDEFENA
 *  Argument to MPU_enable(). Enables priviledged
 *  access to default memory map.                                            */
#define MPU_CTRL_PRIVDEFENA    MPU_CTRL_PRIVDEFENA_Msk

/** @anchor MPU_CTRL_HFNMIENA
 *  Argument to MPU_enable(). Enables MPU during hard fault,
 *  NMI, and FAULTMASK handlers.                                             */
#define MPU_CTRL_HFNMIENA      MPU_CTRL_HFNMIENA_Msk

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/**
 * Size of an MPU region.
 */
typedef enum
{
  mpuRegionSize32b   = 4,        /**< 32   byte region size. */
  mpuRegionSize64b   = 5,        /**< 64   byte region size. */
  mpuRegionSize128b  = 6,        /**< 128  byte region size. */
  mpuRegionSize256b  = 7,        /**< 256  byte region size. */
  mpuRegionSize512b  = 8,        /**< 512  byte region size. */
  mpuRegionSize1Kb   = 9,        /**< 1K   byte region size. */
  mpuRegionSize2Kb   = 10,       /**< 2K   byte region size. */
  mpuRegionSize4Kb   = 11,       /**< 4K   byte region size. */
  mpuRegionSize8Kb   = 12,       /**< 8K   byte region size. */
  mpuRegionSize16Kb  = 13,       /**< 16K  byte region size. */
  mpuRegionSize32Kb  = 14,       /**< 32K  byte region size. */
  mpuRegionSize64Kb  = 15,       /**< 64K  byte region size. */
  mpuRegionSize128Kb = 16,       /**< 128K byte region size. */
  mpuRegionSize256Kb = 17,       /**< 256K byte region size. */
  mpuRegionSize512Kb = 18,       /**< 512K byte region size. */
  mpuRegionSize1Mb   = 19,       /**< 1M   byte region size. */
  mpuRegionSize2Mb   = 20,       /**< 2M   byte region size. */
  mpuRegionSize4Mb   = 21,       /**< 4M   byte region size. */
  mpuRegionSize8Mb   = 22,       /**< 8M   byte region size. */
  mpuRegionSize16Mb  = 23,       /**< 16M  byte region size. */
  mpuRegionSize32Mb  = 24,       /**< 32M  byte region size. */
  mpuRegionSize64Mb  = 25,       /**< 64M  byte region size. */
  mpuRegionSize128Mb = 26,       /**< 128M byte region size. */
  mpuRegionSize256Mb = 27,       /**< 256M byte region size. */
  mpuRegionSize512Mb = 28,       /**< 512M byte region size. */
  mpuRegionSize1Gb   = 29,       /**< 1G   byte region size. */
  mpuRegionSize2Gb   = 30,       /**< 2G   byte region size. */
  mpuRegionSize4Gb   = 31        /**< 4G   byte region size. */
} MPU_RegionSize_TypeDef;

/**
 * MPU region access permission attributes.
 */
typedef enum
{
  mpuRegionNoAccess     = 0,  /**< No access what so ever.                   */
  mpuRegionApPRw        = 1,  /**< Priviledged state R/W only.               */
  mpuRegionApPRwURo     = 2,  /**< Priviledged state R/W, User state R only. */
  mpuRegionApFullAccess = 3,  /**< R/W in Priviledged and User state.        */
  mpuRegionApPRo        = 5,  /**< Priviledged R only.                       */
  mpuRegionApPRo_URo    = 6   /**< R only in Priviledged and User state.     */
} MPU_RegionAp_TypeDef;


/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** MPU Region init structure. */
typedef struct
{
  bool                   regionEnable;     /**< MPU region enable.                */
  uint8_t                regionNo;         /**< MPU region number.                */
  uint32_t               baseAddress;      /**< Region baseaddress.               */
  MPU_RegionSize_TypeDef size;             /**< Memory region size.               */
  MPU_RegionAp_TypeDef   accessPermission; /**< Memory access permissions.   */
  bool                   disableExec;      /**< Disable execution.                */
  bool                   shareable;        /**< Memory shareable attribute.       */
  bool                   cacheable;        /**< Memory cacheable attribute.       */
  bool                   bufferable;       /**< Memory bufferable attribute.      */
  uint8_t                srd;              /**< Memory subregion disable bits.    */
  uint8_t                tex;              /**< Memory type extension attributes. */
} MPU_RegionInit_TypeDef;

/** Default configuration of MPU region init structure for flash memory.     */
#define MPU_INIT_FLASH_DEFAULT                                  \
  {                                                             \
    true,                   /* Enable MPU region.            */ \
    0,                      /* MPU Region number.            */ \
    FLASH_MEM_BASE,         /* Flash base address.           */ \
    mpuRegionSize1Mb,       /* Size - Set to max. for EFM32. */ \
    mpuRegionApFullAccess,  /* Access permissions.           */ \
    false,                  /* Execution allowed.            */ \
    false,                  /* Not shareable.                */ \
    true,                   /* Cacheable.                    */ \
    false,                  /* Not bufferable.               */ \
    0,                      /* No subregions.                */ \
    0                       /* No TEX attributes.            */ \
  }


/** Default configuration of MPU region init structure for sram memory.      */
#define MPU_INIT_SRAM_DEFAULT                                   \
  {                                                             \
    true,                   /* Enable MPU region.            */ \
    1,                      /* MPU Region number.            */ \
    RAM_MEM_BASE,           /* SRAM base address.            */ \
    mpuRegionSize128Kb,     /* Size - Set to max. for EFM32. */ \
    mpuRegionApFullAccess,  /* Access permissions.           */ \
    false,                  /* Execution allowed.            */ \
    true,                   /* Shareable.                    */ \
    true,                   /* Cacheable.                    */ \
    false,                  /* Not bufferable.               */ \
    0,                      /* No subregions.                */ \
    0                       /* No TEX attributes.            */ \
  }


/** Default configuration of MPU region init structure for onchip peripherals.*/
#define MPU_INIT_PERIPHERAL_DEFAULT                             \
  {                                                             \
    true,                   /* Enable MPU region.            */ \
    0,                      /* MPU Region number.            */ \
    0,                      /* Region base address.          */ \
    mpuRegionSize32b,       /* Size - Set to minimum         */ \
    mpuRegionApFullAccess,  /* Access permissions.           */ \
    true,                   /* Execution not allowed.        */ \
    true,                   /* Shareable.                    */ \
    false,                  /* Not cacheable.                */ \
    true,                   /* Bufferable.                   */ \
    0,                      /* No subregions.                */ \
    0                       /* No TEX attributes.            */ \
  }


/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/


void MPU_ConfigureRegion(MPU_RegionInit_TypeDef *init);


/***************************************************************************//**
 * @brief
 *   Disable the MPU
 * @details
 *   Disable MPU and MPU fault exceptions.
 ******************************************************************************/
static __INLINE void MPU_Disable(void)
{
  SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;      /* Disable fault exceptions */
  MPU->CTRL  &= ~MPU_CTRL_ENABLE_Msk;            /* Disable the MPU */
}


/***************************************************************************//**
 * @brief
 *   Enable the MPU
 * @details
 *   Enable MPU and MPU fault exceptions.
 * @param[in] flags
 *   Use a logical OR of @ref MPU_CTRL_PRIVDEFENA and
 *   @ref MPU_CTRL_HFNMIENA as needed.
 ******************************************************************************/
static __INLINE void MPU_Enable(uint32_t flags)
{
  EFM_ASSERT(!(flags & ~(MPU_CTRL_PRIVDEFENA_Msk |
                         MPU_CTRL_HFNMIENA_Msk |
                         MPU_CTRL_ENABLE_Msk)));

  MPU->CTRL   = flags | MPU_CTRL_ENABLE_Msk;     /* Enable the MPU */
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;       /* Enable fault exceptions */
}


/** @} (end addtogroup MPU) */
/** @} (end addtogroup EFM32_Library) */

#ifdef __cplusplus
}
#endif

#endif /* defined(__MPU_PRESENT) && (EBI_COUNT == 1) */

#endif /* __EFM32_MPU_H */
