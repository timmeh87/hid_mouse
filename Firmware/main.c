/* Includes ------------------------------------------------------------------------------------------------*/


#include "ht32_board_config.h"
#include "ht32_usbd_core.h"
#include "ht32_usbd_class.h"
#include "ht32_usbd_descriptor.h"
#include "pixart.h"
#include "mouse.h"

/* Global variables ----------------------------------------------------------------------------------------*/
//usb stuff
__ALIGN4 USBDCore_TypeDef gUSBCore;
USBD_Driver_TypeDef gUSBDriver;

MouseSystem_t SYSTEM = {0};

//cpi settings order is:
// actual CPI, 
// number of LED blinks, 
// LED color
#define NUM_CPI_STEPS 8
u16 cpi_settings[3][NUM_CPI_STEPS] = {
	{400,		600,		800,			1000,			1200,			1400,			1600,		3200},
	{1,			3,			1,				3,				1,				3,				1,			3},
	{COLOR_RED, COLOR_RED,	COLOR_YELLOW, 	COLOR_YELLOW,	COLOR_GREEN, 	COLOR_GREEN,	COLOR_CYAN, COLOR_CYAN}
};

static u8 get_next_cpi(void)
{
	SYSTEM.set_cpi++;
	if (SYSTEM.set_cpi >= NUM_CPI_STEPS) SYSTEM.set_cpi = 0;
	return SYSTEM.set_cpi;
}

static void blink_cpi(void)
{
	my_RGBBlinkColor(
		cpi_settings[CPI_LED_BLINKS][SYSTEM.set_cpi], 
		cpi_settings[CPI_LED_COLOR][SYSTEM.set_cpi], COLOR_BLACK, 
		100);
}

static void settings_update_task(void)
{
	//do the long press by polling so we can get a long-press virtual "on button down" event after DPI_LONG_PRESS_TICKS
	// instead of waiting for the button-up interrupt which is until the user decides to stop pressing.
	// this way gives immediate visual feedback after the long press duration
	{
		u32 dpidowntime_ = SYSTEM.dpiDownTime; //there is a race between when dpiDownTime is used twice below and the interrupt. 
		// the temporary variable downtime_ fixes it so phantom long presses are not detected
		if (dpidowntime_ && !SYSTEM.dpiButtonUp && ((get_systick() - dpidowntime_) > DPI_LONG_PRESS_TICKS))
		{
			SYSTEM.dpiDownTime = 0;
			SYSTEM.dpiLongPress = true;
			SYSTEM.dpiButtonUp = true; //simulate the button release to prevent re-entry
		}
	}
	
	switch(SYSTEM.settings_state)
	{
		//normal operation, press dpi button to enter settings
		case MOUSE_MODE:
		{			
			my_RGBSetColor(COLOR_BLACK);
			if (SYSTEM.dpiShortPress == true)
			{				
				SYSTEM.dpiShortPress = false;
				SYSTEM.set_cpi = get_next_cpi();
				pixart_set_cpi(cpi_settings[CPI_VALUE][SYSTEM.set_cpi]);
							
				blink_cpi();
				
				SaveMouseSettings_t settings = LoadSettings();
				settings.cpi = SYSTEM.set_cpi;
				SaveSettings(settings);
			}
			if (SYSTEM.dpiLongPress == true)
			{
				SYSTEM.dpiLongPress = false;
				SYSTEM.settings_state = SETTING_MODE;
			}
		}
		break;
		
		//after a long press in mouse mode, led is white and more settings available
		case SETTING_MODE:
		{
			my_RGBSetColor(COLOR_WHITE);
			if (SYSTEM.dpiShortPress == true)
			{
				SYSTEM.dpiShortPress = false;
				SYSTEM.settings_state = MOUSE_MODE;
			}
			if (SYSTEM.dpiLongPress == true)
			{
				SYSTEM.dpiLongPress = false;
				SYSTEM.settings_state = MOUSE_MODE;
			}
			if (SYSTEM.MouseState.Button.bits.B4) //side button
			{
				uint8_t num_blink;
				switch(SYSTEM.set_report_period)
				{
					case 0:
					{
						SYSTEM.set_report_period = 1;
						num_blink = 2;
					} break;
					case 1:
					{
						SYSTEM.set_report_period = 2;
						num_blink = 3;
					} break;
					case 2:
					{
						SYSTEM.set_report_period = 3;
						num_blink = 4;
					} break;
					default:
					case 3:
					{	SYSTEM.set_report_period = 0;
						num_blink = 1;
					} break;
				}		

				my_RGBBlinkColor(
					num_blink, 
					COLOR_BLUE, COLOR_WHITE, 
					150);				
				
				SaveMouseSettings_t settings = LoadSettings();
				settings.report_period = SYSTEM.set_report_period;
				SaveSettings(settings);
				SYSTEM.dpiLongPress = false;
				SYSTEM.dpiShortPress = false;
			}
			if (SYSTEM.MouseState.Button.bits.B5) //side button
			{				
				uint8_t num_blink;
				switch(SYSTEM.set_liftoff)
				{
					case 2:
					{
						pixart_set_liftoff(3);
						num_blink = 2;
					} break;
					
					default:
					case 3:
					{
						pixart_set_liftoff(2);
						num_blink = 1;
					} break;
				}		
							
				my_RGBBlinkColor(
						num_blink, 
						COLOR_MAGENTA, COLOR_WHITE, 
						150);
				
				SaveMouseSettings_t settings = LoadSettings();
				settings.liftoff = SYSTEM.set_liftoff;
				SaveSettings(settings);
				SYSTEM.dpiLongPress = false;
				SYSTEM.dpiShortPress = false;
			}
			if ((SYSTEM.MouseState.Button.bits.B1) && (SYSTEM.MouseState.Button.bits.B2)) //LB + RB
			{
				SYSTEM.settings_state = LIFOFF_CAL_ARMED;
			}			
		}
		break;
		
		//after pressing RB + LB in settins mode
		case LIFOFF_CAL_ARMED:
		{
			if (SYSTEM.MouseState.Button.bits.B3) //wheel button
			{
				SYSTEM.settings_state = DOING_LIFTOFF_CAL;				
				SYSTEM.dpiLongPress = false;
				SYSTEM.dpiShortPress = false;
				pixart_liftoff_cal_begin();				
			}
			
			//blink red/white
			if (get_systick() & 0x4000) //2^14 * 20us = ~327ms
			{
				my_RGBSetColor(COLOR_WHITE);
			} else	{
				my_RGBSetColor(COLOR_RED);
			}
			
		}
		break;
		
		case DOING_LIFTOFF_CAL:
		{
			//blink green/white
			if (get_systick() & 0x4000) //2^14 * 20us = ~327ms
			{
				my_RGBSetColor(COLOR_WHITE);
			} else	{
				my_RGBSetColor(COLOR_GREEN);
			}
			if ((pixart_liftoff_cal_running() == false) || SYSTEM.dpiLongPress || SYSTEM.dpiShortPress)
			{
				SYSTEM.dpiLongPress = false;
				SYSTEM.dpiShortPress = false;
				if (pixart_liftoff_cal_save())
				{
					SYSTEM.settings_state = MOUSE_MODE;		
				} else {
					SYSTEM.settings_state = LIFTOFF_FAILED;		
				}
			}
		}
		
		break;		
		
		case LIFTOFF_FAILED:
		{
			//blink green/white
			if (get_systick() & 0x1000) //2^14 * 20us = ~327ms
			{
				my_RGBSetColor(COLOR_MAGENTA);
			} else	{
				my_RGBSetColor(COLOR_CYAN);
			}
			if (SYSTEM.dpiLongPress || SYSTEM.dpiShortPress)
			{
				SYSTEM.dpiLongPress = false;
				SYSTEM.dpiShortPress = false;
				SYSTEM.settings_state = MOUSE_MODE;					
			}
		}
		
		break;	
	}
}

int main(void)
{
	CKCU_Configuration();
	GPIO_Configuration();
	my_RGBSet(false, false, false);
	
	// initialize the main control structure
	SYSTEM.set_report_period = 0;
	SYSTEM.set_liftoff = 3;
	
	SYSTEM.usbIsLowPowerAllowed = TRUE;
	SYSTEM.autoTrimSaved = FALSE;
	
	 /* SYSTICK configuration */
	SYSTICK_ClockSourceConfig(SYSTICK_SRC_STCLK);       // Default : CK_AHB/8 - 48mhz
	SYSTICK_SetReloadValue(SystemCoreClock / 8/ 50000); // (CK_AHB/8/50000) = 20us 
	SYSTICK_IntConfig(ENABLE);                          // Enable SYSTICK Interrupt
    SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);


	
	if (SET != GPIO_ReadInBit(KEY4_HT_GPIO, KEY4_BUTTON_GPIO_PIN))
	{
		my_RGBSet(true, true, true);
		Delay(50*1000);
		my_RGBSet(false, false, false);
	}
	RETARGET_Configuration();
	USB_Configuration();
	SPI_Configuration();  	
	CKCU_LoadTrimValue();
	HT32F_DVB_USBConnect();
	InitPixartSensor();
	my_LEDOff();
	
	SaveMouseSettings_t flash_settings = LoadSettings();
	if (flash_settings.Header == 0x5a5b)
	{
		if (flash_settings.SQUAL_cal)
		{
			pixart_set_squal(flash_settings.SQUAL_cal);
		}
		if (flash_settings.threshold_cal)
		{
			pixart_set_threshold(flash_settings.threshold_cal);
		}
		if (flash_settings.liftoff)
		{
			pixart_set_liftoff(flash_settings.liftoff);			
		}
		if (flash_settings.report_period < 4)
		{
			SYSTEM.set_report_period = flash_settings.report_period;
		}
				
		//induce a setting change to the saved step so it will display on startup and calculate CPI for us
		if (flash_settings.cpi < NUM_CPI_STEPS)
		{
			pixart_set_cpi(cpi_settings[CPI_VALUE][flash_settings.cpi]);
			SYSTEM.set_cpi = flash_settings.cpi;			
		} else {
			SYSTEM.set_cpi = 2;
		}	
	}
	else
	{
		pixart_set_liftoff(SYSTEM.set_liftoff);		
		SaveMouseSettings_t fresh_flash_settings = {0};
		SaveSettings(fresh_flash_settings);
	}
	blink_cpi();
	SYSTEM.dpiShortPress = false;
	SYSTEM.dpiLongPress = false;
	SYSTEM.need_update=true;
	while (1)
	{
		settings_update_task();
		USBDCore_MainRoutine(&gUSBCore);  
		UpdateAutoTrimValue(); 
	}
}

void CKCU_Configuration(void)
{
#if 1
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.USBD       = 1;
	CKCUClock.Bit.PB         = 1;
	CKCUClock.Bit.PC         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
#endif
}

/*********************************************************************************************************//**
  * @brief  Configure the system clocks.
  * @retval None
  ***********************************************************************************************************/
void CKCU_LoadTrimValue(void)
{
  SaveAutoTrim_t *pSaveTrim;

  /* Getting  the trim value from SAVE_AUTO_TRIM_ADDRESS                                                         */
  u32 uTrimValue = rw(SAVE_AUTO_TRIM_ADDRESS);

  pSaveTrim = (SaveAutoTrim_t*)&uTrimValue;
  /* Check the trim value is valid                                                                               */
  if(pSaveTrim->Header != SAVE_AUTO_TRIM_HEAD_KEY)
  {
  /* the trim value isn't valid                                                                                  */
    SYSTEM.autoTrimValue = 0;
    return;
  }
  else
  {
  /* the trim value is valid, then reload the trim value to HSICR                                                */
    printf("Load the Trim Value...\r\n\t\t\tTrim Value : 0x%03X\r\n",pSaveTrim->TrimValue);
    SYSTEM.autoTrimValue = pSaveTrim->TrimValue;
    HT_CKCU->HSICR = ((pSaveTrim->TrimValue << 16) | (1ul << 4) | (1ul << 0));
  }
}

void UpdateAutoTrimValue(void)
{
  /* 
    1. Check HSI auto-trim is ready.
    2. Check the Trim Value has saved to SAVE_AUTO_TRIM_ADDRESS. Avoid write to flash frequently. 
  */
  if(SYSTEM.autoTrimSaved == TRUE)
  {
    return;
  }
  if (CKCU_HSIAutoTrimIsReady() == TRUE)
  {
    FLASH_State FLASHState;
    u32 uBuffer = 0;
    SaveAutoTrim_t *pSaveTrim = (SaveAutoTrim_t*)&uBuffer;
    pSaveTrim->TrimValue = (HT_CKCU->HSICR & 0x00FF0000) >> 16;
    pSaveTrim->Header = SAVE_AUTO_TRIM_HEAD_KEY;
    printf("Detect a new Trim Value...\r\n\t\t\tTrim Value : 0x%03X\r\n",pSaveTrim->TrimValue);
    printf("Save the new Trim Value...\r\n");

    if(SYSTEM.autoTrimValue == pSaveTrim->TrimValue)
    {
      /*If Trim Value isn't changed,The system will not write Trim value to data area. 
        The purpose is to reduce flash writes frequently.                                                      */
      FLASHState = FLASH_COMPLETE;
    }
    else
    {
      /* Write Trim Value to data area.                                                                        */
      FLASHState = FLASH_EraseOptionByte();
      if(FLASHState != FLASH_COMPLETE)
      {
        printf("\t\t\tErrase error : %d",FLASHState);
        SYSTEM.autoTrimSaved = FALSE;
        return ;
      }
      FLASHState = FLASH_ProgramWordData(SAVE_AUTO_TRIM_ADDRESS, uBuffer);
    }
    if(FLASHState != FLASH_COMPLETE)
    {
      printf("\t\t\tProgram error : %d",FLASHState);
      SYSTEM.autoTrimSaved = FALSE;
    }
    else
    {
      printf("\t\t\tFinish\r\n");
      SYSTEM.autoTrimSaved = TRUE;
    }
  }
}

static void my_ClockConfig(u32 GpioId)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	u8 RegCK[GPIO_PORT_NUM] = {0};
	RegCK[GpioId] = 1;

	CKCUClock.Bit.PA         = RegCK[0];
	CKCUClock.Bit.PB         = RegCK[1];
	#if (LIBCFG_GPIOC)
	CKCUClock.Bit.PC         = RegCK[2];
	#endif
	#if (LIBCFG_GPIOD)
	CKCUClock.Bit.PD         = RegCK[3];
	#endif
	#if (LIBCFG_GPIOE)
	CKCUClock.Bit.PE         = RegCK[4];
	#endif
	#if (LIBCFG_GPIOF)
	CKCUClock.Bit.PF         = RegCK[5];
	#endif
	CKCUClock.Bit.AFIO       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
void my_GPIOOn(u32 bGpioId, u32 uGPIOPin)
{
	GPIO_PORT[bGpioId]->SRR = uGPIOPin;
}
void my_GPIOOff(u32 bGpioId, u32 uGPIOPin)
{
	GPIO_PORT[bGpioId]->RR = uGPIOPin;
}
void my_LEDOff(void)
{
	my_GPIOOff(GPIO_PB, GPIO_PIN_6);
}
void my_LEDOn(void)
{
	my_GPIOOn(GPIO_PB, GPIO_PIN_6);
}

void my_RGBSet(bool r, bool g, bool b)
{
	r ? my_GPIOOff(GPIO_PA, GPIO_PIN_5) : my_GPIOOn(GPIO_PA, GPIO_PIN_5);
	b ? my_GPIOOff(GPIO_PA, GPIO_PIN_6) : my_GPIOOn(GPIO_PA, GPIO_PIN_6);
	g ? my_GPIOOff(GPIO_PA, GPIO_PIN_7) : my_GPIOOn(GPIO_PA, GPIO_PIN_7);
}

void my_RGBSetColor(colors_e color)
{
	switch (color)
	{
		case COLOR_RED: 	my_RGBSet(true, false, false); 	break;
		case COLOR_YELLOW: 	my_RGBSet(true, true, false);	break;
		case COLOR_GREEN: 	my_RGBSet(false, true, false);	break;
		case COLOR_CYAN: 	my_RGBSet(false, true, true);	break;
		case COLOR_BLUE: 	my_RGBSet(false, false, true);	break;
		case COLOR_MAGENTA: my_RGBSet(true, false, true);	break;
		case COLOR_WHITE: 	my_RGBSet(true, true, true);	break;
		case COLOR_BLACK: 	my_RGBSet(false, false, false);	break;
	}
}

void my_RGBBlinkColor(u8 n_blinks, colors_e color1, colors_e color2, u16 period_ms)
{
	for (u8 b=0; b<n_blinks; b++)
	{
		my_RGBSetColor(color1);
		Delay_ms(period_ms);
		my_RGBSetColor(color2);
		Delay_ms(period_ms);
	}
}



static void my_GPIOInit(u32 bGpioId, u32 uGPIOPin)
{
	HT_GPIO_TypeDef* GPIOx = GPIO_PORT[bGpioId];

	/* Enable the GPIO_LED Clock                                                                              */
	/* Enable the AFIO Clock                                                                                  */
	my_ClockConfig(bGpioId);

	/* Configure the AFIO mode                                                                                */
	HT32F_DVB_GPxConfig(bGpioId, uGPIOPin, AFIO_FUN_GPIO);

	/* off by default                                                                                  */
	my_GPIOOff(bGpioId, uGPIOPin);

	/* Configure the GPIO pin                                                                                 */
	GPIO_PullResistorConfig(GPIOx, uGPIOPin, GPIO_PR_DISABLE);
	GPIO_DriveConfig(GPIOx, uGPIOPin, GPIO_DV_4MA);
	GPIO_DirectionConfig(GPIOx, uGPIOPin, GPIO_DIR_OUT);
}

void GPIO_Configuration(void)
{
	/*--------------------------------------------------------------------------------------------------------*/
	/* Configure GPIO as input for button                                                                     */
	/*--------------------------------------------------------------------------------------------------------*/
	//HT32F_DVB_PBInit(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
	HT32F_DVB_PBInit(BUTTON_KEY1, BUTTON_MODE_EXTI);
	HT32F_DVB_PBInit(BUTTON_KEY2, BUTTON_MODE_EXTI);
	HT32F_DVB_PBInit(BUTTON_KEY3, BUTTON_MODE_EXTI);
	HT32F_DVB_PBInit(BUTTON_KEY4, BUTTON_MODE_EXTI);
	//HT32F_DVB_PBInit(BUTTON_KEY5, BUTTON_MODE_EXTI); //middle mouse button (not wheel)
	HT32F_DVB_PBInit(BUTTON_KEY6, BUTTON_MODE_EXTI);
	HT32F_DVB_PBInit(BUTTON_KEY7, BUTTON_MODE_EXTI);
	HT32F_DVB_PBInit(BUTTON_KEY8, BUTTON_MODE_EXTI);
	HT32F_DVB_PBInit(BUTTON_KEY9, BUTTON_MODE_EXTI);
	
	GPIO_PullResistorConfig(HTCFG_KEY3_GPIO_PORT, HTCFG_KEY3_GPIO_PIN, GPIO_PR_UP);

	//B5 is the reset pin, we will hold it in reset for now
	my_GPIOInit(GPIO_PB, GPIO_PIN_5);
	my_GPIOOff(GPIO_PB, GPIO_PIN_5); //assert reset
	
	//B6 is internal LED, a5/6/7 is rgb
	my_GPIOInit(GPIO_PB, GPIO_PIN_6);	
	my_GPIOInit(GPIO_PA, GPIO_PIN_5);
	my_GPIOInit(GPIO_PA, GPIO_PIN_6);
	my_GPIOInit(GPIO_PA, GPIO_PIN_7);
	
	//A3 is the chip select pin
	my_GPIOInit(GPIO_PA, GPIO_PIN_3);
	my_GPIOOn(GPIO_PA, GPIO_PIN_3); //deassert select
}

/*********************************************************************************************************//**
  * @brief  Configure USB.
  * @retval None
  ***********************************************************************************************************/
void USB_Configuration(void)
{
	#if (LIBCFG_CKCU_USB_PLL)
	USBPLL_Configuration();
	#endif

	#if (LIBCFG_PWRCU_VREG)
	USBVRG_Configuration();                               /* Voltage of USB setting                           */
	#endif

	gUSBCore.pDriver = (u32 *)&gUSBDriver;                /* Initiate memory pointer of USB driver            */
	gUSBCore.Power.CallBack_Suspend.func  = Suspend;      /* Install suspend call back function into USB core */
	//gUSBCore.Power.CallBack_Suspend.uPara = (u32)NULL;

	USBDDesc_Init(&gUSBCore.Device.Desc);                 /* Initiate memory pointer of descriptor            */
	USBDClass_Init(&gUSBCore.Class);                      /* Initiate USB Class layer                         */
	USBDCore_Init(&gUSBCore);                             /* Initiate USB Core layer                          */
	
	//must always use auto trim on HSI
	CKCU_HSIAutoTrimClkConfig(CKCU_ATC_USB);
	CKCU_HSIAutoTrimCmd(ENABLE);

	NVIC_EnableIRQ(USB_IRQn);                             /* Enable USB device interrupt                      */
}

#if (LIBCFG_CKCU_USB_PLL)
/*********************************************************************************************************//**
 * @brief  Configure USB PLL
 * @retval None
 ************************************************************************************************************/
void USBPLL_Configuration(void)
{
  { /* USB PLL configuration                                                                                */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    CKCU_PLLInitTypeDef PLLInit;

    PLLInit.ClockSource = CKCU_PLLSRC_HSE;  // CKCU_PLLSRC_HSE or CKCU_PLLSRC_HSI
    #if (LIBCFG_CKCU_USB_PLL_96M)
    PLLInit.CFG = CKCU_USBPLL_8M_96M;
    #else
    PLLInit.CFG = CKCU_USBPLL_8M_48M;
    #endif
    PLLInit.BYPASSCmd = DISABLE;
    CKCU_USBPLLInit(&PLLInit);
  }

  CKCU_USBPLLCmd(ENABLE);

  while (CKCU_GetClockReadyStatus(CKCU_FLAG_USBPLLRDY) == RESET);
  CKCU_USBClockConfig(CKCU_CKUSBPLL);
}
#endif

#if (LIBCFG_PWRCU_VREG)
/*********************************************************************************************************//**
 * @brief  Configure USB Voltage 
 * @retval None
 ************************************************************************************************************/
void USBVRG_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  PWRCU_SetVREG(PWRCU_VREG_3V3);

  /* !!! NOTICE !!!
     USB LDO should be enabled (PWRCU_VREG_ENABLE) if the MCU VDD > 3.6 V.
  */
  PWRCU_VREGConfig(PWRCU_VREG_BYPASS);
}
#endif

#define REMOTE_WAKEUP      (0)
/*********************************************************************************************************//**
  * @brief  Suspend call back function which enter DeepSleep1
  * @param  uPara: Parameter for Call back function
  * @retval None
  ***********************************************************************************************************/
void Suspend(u32 uPara)
{
  #if (REMOTE_WAKEUP == 1)
  u32 IsRemoteWakeupAllowed;
  #endif

  if (SYSTEM.usbIsLowPowerAllowed)
  {

    #if (REMOTE_WAKEUP == 1)
    /* Disable EXTI interrupt to prevent interrupt occurred after wakeup                                    */
    EXTI_IntConfig(KEY1_BUTTON_EXTI_CHANNEL, DISABLE);
    IsRemoteWakeupAllowed = USBDCore_GetRemoteWakeUpFeature(&gUSBCore);

    if (IsRemoteWakeupAllowed == TRUE)
    {
      /* Enable EXTI wake event and clear wakeup flag                                                       */
      EXTI_WakeupEventConfig(KEY1_BUTTON_EXTI_CHANNEL, EXTI_WAKEUP_LOW_LEVEL, ENABLE);
      EXTI_ClearWakeupFlag(KEY1_BUTTON_EXTI_CHANNEL);
    }
    #endif

    __DBG_USBPrintf("%06ld >DEEPSLEEP\r\n", ++__DBG_USBCount);

    // Add your procedure here which disable related IO to reduce power consumption
    // ..................
    //

    /* For Bus powered device, you must enter DeepSleep1 when device has been suspended. For self-powered   */
    /* device, you may decide to enter DeepSleep1 or not depended on your application.                      */

    /* For the convenient during debugging and evaluation stage, the USBDCore_LowPower() is map to a null   */
    /* function by default. In the real product, you must map this function to the low power function of    */
    /* firmware library by setting USBDCORE_ENABLE_LOW_POWER as 1 (in the ht32fxxxx_usbdconf.h file).       */
    USBDCore_LowPower();

    // Add your procedure here which recovery related IO for application
    // ..................
    //

    __DBG_USBPrintf("%06ld <DEEPSLEEP\r\n", ++__DBG_USBCount);

    #if (REMOTE_WAKEUP == 1)
    if (EXTI_GetWakeupFlagStatus(KEY1_BUTTON_EXTI_CHANNEL) == SET)
    {
      __DBG_USBPrintf("%06ld WAKEUP\r\n", ++__DBG_USBCount);
      if (IsRemoteWakeupAllowed == TRUE && USBDCore_IsSuspend(&gUSBCore) == TRUE)
      {
        USBDCore_TriggerRemoteWakeup();
      }
    }

    if (IsRemoteWakeupAllowed == TRUE)
    {
      /* Disable EXTI wake event and clear wakeup flag                                                      */
      EXTI_WakeupEventConfig(KEY1_BUTTON_EXTI_CHANNEL, EXTI_WAKEUP_LOW_LEVEL, DISABLE);
      EXTI_ClearWakeupFlag(KEY1_BUTTON_EXTI_CHANNEL);
    }

    /* Clear EXTI edge flag and enable EXTI interrupt                                                       */
    EXTI_ClearEdgeFlag(KEY1_BUTTON_EXTI_CHANNEL);
    EXTI_IntConfig(KEY1_BUTTON_EXTI_CHANNEL, ENABLE);
    #endif
  }

  return;
}


/*********************************************************************************************************//**
  * @brief  Configure the SPI INT ports.
  * @retval None
  ***********************************************************************************************************/
void SPIINT_Configuration(void)
{

	EXTI_InitTypeDef EXTI_InitStruct;

	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	HTCFG_INT_CLK(CKCUClock) = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);

	AFIO_GPxConfig(HTCFG_INT_GPIO_ID, HTCFG_INT_GPIO_PIN, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(GPIO_PORT[HTCFG_INT_GPIO_ID], HTCFG_INT_GPIO_PIN, GPIO_DIR_IN);
	GPIO_PullResistorConfig(GPIO_PORT[HTCFG_INT_GPIO_ID], HTCFG_INT_GPIO_PIN, GPIO_PR_UP);
	GPIO_InputConfig(GPIO_PORT[HTCFG_INT_GPIO_ID], HTCFG_INT_GPIO_PIN, ENABLE);

	EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_0;
	EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;
	EXTI_InitStruct.EXTI_DebounceCnt = 0;
	EXTI_InitStruct.EXTI_IntType = EXTI_NEGATIVE_EDGE;
	EXTI_Init(&EXTI_InitStruct);
	AFIO_EXTISourceConfig(AFIO_EXTI_CH_0, HTCFG_INT_AFIO_ESS);

	EXTI_IntConfig(EXTI_CHANNEL_0, ENABLE);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

/*********************************************************************************************************//**
  * @brief  Configure the SPI ports.
  * @retval None
  ***********************************************************************************************************/
void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;

	CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
	CKCUClock.Bit.SPI0       = 1;
	CKCUClock.Bit.SPI1       = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);

	AFIO_GPxConfig(HTCFG_MASTER_SPI_SCK_GPIO_ID, HTCFG_MASTER_SPI_SCK_AFIO_PIN, AFIO_FUN_SPI);
	AFIO_GPxConfig(HTCFG_MASTER_SPI_MISO_GPIO_ID, HTCFG_MASTER_SPI_MISO_AFIO_PIN, AFIO_FUN_SPI);
	AFIO_GPxConfig(HTCFG_MASTER_SPI_MOSI_GPIO_ID, HTCFG_MASTER_SPI_MOSI_AFIO_PIN, AFIO_FUN_SPI);

	SPI_InitStructure.SPI_Mode = SPI_MASTER;
	SPI_InitStructure.SPI_FIFO = SPI_FIFO_ENABLE;
	SPI_InitStructure.SPI_DataLength = SPI_DATALENGTH_8;
	SPI_InitStructure.SPI_SELMode = SPI_SEL_HARDWARE;
	SPI_InitStructure.SPI_SELPolarity = SPI_SELPOLARITY_LOW;
	SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_SECOND;
	SPI_InitStructure.SPI_RxFIFOTriggerLevel = 1;
	SPI_InitStructure.SPI_TxFIFOTriggerLevel = 0;
	SPI_InitStructure.SPI_ClockPrescaler = 4;
	SPI_Init(HTCFG_MASTER_SPI, &SPI_InitStructure);

	SPI_SELOutputCmd(HTCFG_MASTER_SPI, ENABLE);

	SPI_IntConfig(HTCFG_MASTER_SPI, SPI_INT_RXBNE, ENABLE);
	NVIC_EnableIRQ(HTCFG_MASTER_SPI_IRQ);

	SPI_Cmd(HTCFG_MASTER_SPI, ENABLE);
}

void Delay(u32 nTime)
{
	uint32_t DelayTime;
	uint32_t now = get_systick();
	SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);

	DelayTime = nTime + now;

	while(get_systick() < (DelayTime + 1)); //this provides slightly less accurate delay than the method of resetting the timer so we throw on an extra period.
	//the actual delay has an error of (plus 20 / minus 0) microseconds
}

void Delay_ms(u32 nTime)
{
	uint32_t DelayTime;
	uint32_t now = get_systick();
	SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);

	DelayTime = nTime*50 + now;

	while(get_systick() < (DelayTime + 1))
	{		
		USBDCore_MainRoutine(&gUSBCore);  
	}
}


//note: only the calibration settings can actually be set externally
bool SaveSettings(SaveMouseSettings_t settings)
{
	FLASH_State FLASHState = FLASH_ErasePage(SAVE_SETTINGS_ADDRESS);
	   
    u32 *pSave = (u32*)&settings;
	settings.Header = 0x5a5b;
	settings.cpi = SYSTEM.set_cpi;
	settings.liftoff = SYSTEM.set_liftoff;
	settings.report_period = SYSTEM.set_report_period;
     
	if(FLASHState != FLASH_COMPLETE)
	{
		//printf("\t\t\tErrase error : %d",FLASHState);
		return false;
	}
	
	for (int b =0; b<6; b++)
	{
		FLASHState = FLASH_ProgramWordData(SAVE_SETTINGS_ADDRESS + (b<<2), *(pSave+b));
		if(FLASHState != FLASH_COMPLETE) break;
	}
  
    if(FLASHState != FLASH_COMPLETE)
    {
		return false;
    }
    else
    {		
		return true;
    }
}

SaveMouseSettings_t LoadSettings(void)
{
	SaveMouseSettings_t settings = {0};
	u32 *pSave = (u32*)&settings;
	
	for (int b =0; b<6; b++)
	{
		pSave[b] = rw(SAVE_SETTINGS_ADDRESS + (b<<2));
	}
	return settings;
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif
