/*********************************************************************************************************//**
 * @file    SPI/Master/ht32_board_config.h
 * @version $Rev:: 5575         $
 * @date    $Date:: 2021-08-18 #$
 * @brief   The header file of board configuration.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __HT32_BOARD_CONFIG_H
#define __HT32_BOARD_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

//added this here instead of making some kind of "main" header
#include "ht32.h"
void my_RGBSet(bool r, bool g, bool b);
	
#define _HTCFG_KEY3_GPIOX                       C
#define _HTCFG_KEY3_GPION                       4
#define HTCFG_KEY3_EXTI_IRQn                   EXTI4_IRQn
#define HTCFG_KEY3_GPIO_CK                        STRCAT2(P,              _HTCFG_KEY3_GPIOX)
#define HTCFG_KEY3_GPIO_ID                        STRCAT2(GPIO_P,         _HTCFG_KEY3_GPIOX)
#define HTCFG_KEY3_AFIO_PIN                       STRCAT2(AFIO_PIN_,      _HTCFG_KEY3_GPION)
#define HTCFG_KEY3_GPIO_PORT                      STRCAT2(HT_GPIO,        _HTCFG_KEY3_GPIOX)
#define HTCFG_KEY3_GPIO_PIN                       STRCAT2(GPIO_PIN_,      _HTCFG_KEY3_GPION)
#define HTCFG_KEY3_AFIO_EXTI_CH                   STRCAT2(AFIO_EXTI_CH_,  _HTCFG_KEY3_GPION)
#define HTCFG_KEY3_AFIO_ESS                       STRCAT2(AFIO_ESS_P,     _HTCFG_KEY3_GPIOX)
#define HTCFG_KEY3_EXTI_CH                        STRCAT2(EXTI_CHANNEL_,  _HTCFG_KEY3_GPION)
// end of what tim added


/* Settings ------------------------------------------------------------------------------------------------*/
#if defined(USE_HT32F52230_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PA)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PA)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PA)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_2)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_3)
#endif

#if defined(USE_HT32F52241_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_10)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_11)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F52253_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PC)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PC)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PC)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_7)
#endif

#if defined(USE_HT32F52341_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PC)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PC)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PC)
  
  #define HTCFG_MASTER_SPI                     (HT_SPI1)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI1_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_0)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_1)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_2)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_3)
#endif

#if defined(USE_HT32F52352_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_4)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PA)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PA)

  #define HTCFG_MASTER_SPI                     (HT_SPI1)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI1_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_0)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_1)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_2)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_3)
#endif

#if defined(USE_HT32F0008_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F50230_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F50241_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F52354_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F0006_DVB)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PC)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PC)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PC)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_6)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_7)
#endif

#if defined(USE_HT32F57341_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F57352_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F50343_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_3)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PB)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F52367_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PE)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_1)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_9)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_6)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_7)
#endif

#if defined(USE_HT32F65240_DVB)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PA)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PA)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PA)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_2)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_3)
#endif

#if defined(USE_HT32F65240_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PC)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PC)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PC)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PC)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PC)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_6)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PC)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_7)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_7)
#endif

#if defined(USE_HT32F65232_SK)  //PRELIMINARY_NOT_TEST
  #define HTCFG_INT_GPIO_ID                    (GPIO_PC)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PC)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PC)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PC)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_7)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PC)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PC)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PC)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_6)
#endif

#if defined(USE_HT32F61355_SK) || defined(USE_HT32F61356_SK) || defined(USE_HT32F61357_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PC)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PC)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PC)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_4)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_5)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_6)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_2)
#endif

#if defined(USE_HT32F54241_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PA)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PA)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PA)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PC)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_0)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_10)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_6)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PA)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_7)
#endif

#if defined(USE_HT32F54253_SK)
  #define HTCFG_INT_GPIO_ID                    (GPIO_PB)
  #define HTCFG_INT_GPIO_PIN                   (GPIO_PIN_0)
  #define HTCFG_INT_CLK(CK)                    (CK.Bit.PB)
  #define HTCFG_INT_AFIO_ESS                   (AFIO_ESS_PB)

  #define HTCFG_MASTER_SPI                     (HT_SPI0)
  #define HTCFG_MASTER_SPI_IRQ                 (SPI0_IRQn)

  #define HTCFG_MASTER_SPI_SCK_GPIO_ID         (GPIO_PC)
  #define HTCFG_MASTER_SPI_SCK_AFIO_PIN        (AFIO_PIN_0)

  #define HTCFG_MASTER_SPI_MOSI_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MOSI_AFIO_PIN       (AFIO_PIN_10)

  #define HTCFG_MASTER_SPI_MISO_GPIO_ID        (GPIO_PA)
  #define HTCFG_MASTER_SPI_MISO_AFIO_PIN       (AFIO_PIN_11)

  #define HTCFG_MASTER_SPI_SEL_GPIO_ID         (GPIO_PB)
  #define HTCFG_MASTER_SPI_SEL_AFIO_PIN        (GPIO_PIN_15)
#endif

#ifdef __cplusplus
}
#endif

#endif
