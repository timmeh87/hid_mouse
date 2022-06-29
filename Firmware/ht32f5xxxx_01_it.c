
/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_usbd_core.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "pixart.h"
#include "mouse.h"

extern MouseSystem_t SYSTEM;

void HardFault_Handler(void)
{
  #ifndef IS_PRODUCTION

  static vu32 Continue = 0;
  /*--------------------------------------------------------------------------------------------------------*/
  /* For development FW, MCU run into the while loop when the hardfault occurred.                           */
  /* 1. Stack Checking                                                                                      */
  /*    When a hard fault exception occurs, MCU push following register into the stack (main or process     */
  /*    stack). Confirm R13(SP) value in the Register Window and typing it to the Memory Windows, you can   */
  /*    check following register, especially the PC value (indicate the last instruction before hard fault).*/
  /*    SP + 0x00    0x04    0x08    0x0C    0x10    0x14    0x18    0x1C                                   */
  /*           R0      R1      R2      R3     R12      LR      PC    xPSR                                   */
  while (Continue == 0)
  {
  }
  /* 2. Step Out to Find the Clue                                                                           */
  /*    Change the variable "gIsContinue" to any other value than zero in a Local or Watch Window, then     */
  /*    step out the HardFault_Handler to reach the first instruction after the instruction which caused    */
  /*    the hard fault.                                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/

  #else

  /*--------------------------------------------------------------------------------------------------------*/
  /* For production FW, you shall consider to reboot the system when hardfault occurred.                    */
  /*--------------------------------------------------------------------------------------------------------*/
  NVIC_SystemReset();

  #endif
}


void USB_IRQHandler(void)
{
  __ALIGN4 extern USBDCore_TypeDef gUSBCore;
  USBDCore_IRQHandler(&gUSBCore);
	
}

volatile uint32_t systick_count = 0;
void SysTick_Handler(void)
{
  systick_count++;
}
uint32_t get_systick(void)
{
	return systick_count;
}

//this receives bytes from the SPI bus, we only hooked one interrupt and that is the RXB one (receive byte)
void SPI1_IRQHandler(void)
{
  u32 uCount = SPI_GetFIFOStatus(HTCFG_MASTER_SPI, SPI_FIFO_RX);

  while (uCount--)
  {
    SYSTEM.spiReceiveBuffer[SYSTEM.spiReceivedCount++] = SPI_ReceiveData(HTCFG_MASTER_SPI);
    if (SYSTEM.spiReceivedCount == 20)
    {
      SYSTEM.spiReceivedCount=0; //receive buffer rollover. for the pixart sensor we only really get a few bytes at a time.
		//the reading function will reset this counter when it wants to get some data..
		//but during firmware upload this counter just rolls around a few hundred times collecting garbage because every write
		//generates a read on a SPI bus. Yeah I could just disable the interrupt but this is a lot easier with no real downside
    }
  }
}


typedef enum 
{
	ENC_STATE_00,
	ENC_STATE_01,
	ENC_STATE_10,
	ENC_STATE_11
} encoder_states_e;
encoder_states_e last_encoder_state = ENC_STATE_00;
static int8_t current_wheel_motion = 0;

//gets and also zeros out the wheel accumulator
int8_t get_wheel_motion(void)
{
	uint8_t temp = -current_wheel_motion; //invert
	current_wheel_motion = 0;
	return temp;
}

//handlers that fire whenever button state changes
static void mouse_wheel_button_handler(bool keydown)
{	//the mouse wheel switch is always called "middle click"
	SYSTEM.MouseState.Button.bits.B3 = keydown;
}
static void left_click_button_handler(bool keydown)
{
	SYSTEM.MouseState.Button.bits.B1 = keydown;
}
static void right_click_button_handler(bool keydown)
{
	SYSTEM.MouseState.Button.bits.B2 = keydown;
}
static void middle_button_handler(bool keydown)
{
	//gMouseState.Button.bits.B6 = keydown;
}

static void side_button_1_handler(bool keydown)
{
	SYSTEM.MouseState.Button.bits.B5 = keydown;
}
static void side_button_2_handler(bool keydown)
{
	SYSTEM.MouseState.Button.bits.B4 = keydown;
}


static void encoder_transition_handler(void)
{
	bool enc_A = (bool)(SET == GPIO_ReadInBit(KEY6_HT_GPIO, KEY6_BUTTON_GPIO_PIN));
	bool enc_B = (bool)(SET == GPIO_ReadInBit(KEY7_HT_GPIO, KEY7_BUTTON_GPIO_PIN));
	encoder_states_e current_encoder_state = (encoder_states_e)((enc_A * 2) + enc_B); //dirty cast to enum value
	/* QUADRATURE STATE ROTATION:
		00, 01, 11, 10 -> FORWARDS
		00, 10, 11, 01 -> BACKWARDS
	*/
	switch (last_encoder_state)
	{
		case ENC_STATE_00:
		{
			if (current_encoder_state == ENC_STATE_01) {current_wheel_motion++;}
			else if (current_encoder_state == ENC_STATE_10) {current_wheel_motion--;}
		} break;
		case ENC_STATE_01:
		{
			if (current_encoder_state == ENC_STATE_11) {current_wheel_motion++;}
			else if (current_encoder_state == ENC_STATE_00) {current_wheel_motion--;}
		} break;
		case ENC_STATE_10:
		{
			if (current_encoder_state == ENC_STATE_00) {current_wheel_motion++;}
			else if (current_encoder_state == ENC_STATE_11) {current_wheel_motion--;}
		} break;
		case ENC_STATE_11:
		{
			if (current_encoder_state == ENC_STATE_10) {current_wheel_motion++;}
			else if (current_encoder_state == ENC_STATE_01) {current_wheel_motion--;}
		} break;
	}
	
	last_encoder_state=current_encoder_state;		
}

static uint32_t option_down_time;
static void option_button_down_handler(void)
{
	SYSTEM.dpiDownTime = get_systick();
	SYSTEM.dpiButtonUp = false;
	
}
static void option_button_up_handler(void)
{
	SYSTEM.dpiButtonUp = true;	
	if ((get_systick()-SYSTEM.dpiDownTime) < DPI_LONG_PRESS_TICKS) //half second = 50*500 ticks
	{		
		SYSTEM.dpiShortPress = true;
	}
	SYSTEM.dpiDownTime = 0;
}

void EXTI0_1_IRQHandler(void)
{
	if (EXTI_GetEdgeFlag(KEY1_BUTTON_EXTI_CHANNEL))
	{		
		right_click_button_handler((bool)(SET == EXTI_GetEdgeStatus(KEY1_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE)));
	}	
	EXTI_ClearEdgeFlag(KEY1_BUTTON_EXTI_CHANNEL);
	
	if (EXTI_GetEdgeFlag(KEY2_BUTTON_EXTI_CHANNEL))
	{		
		left_click_button_handler((bool)(SET == EXTI_GetEdgeStatus(KEY2_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE)));
	}	
	EXTI_ClearEdgeFlag(KEY2_BUTTON_EXTI_CHANNEL);
}

void EXTI2_3_IRQHandler(void)
{
	if (EXTI_GetEdgeFlag(KEY5_BUTTON_EXTI_CHANNEL)) //key5 is pb2
	{		
		middle_button_handler((bool)(SET == EXTI_GetEdgeStatus(KEY5_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE)));
	}	
	EXTI_ClearEdgeFlag(KEY5_BUTTON_EXTI_CHANNEL);
	
	if (EXTI_GetEdgeFlag(KEY4_BUTTON_EXTI_CHANNEL)) //KEY4 is pb3
	{		
		mouse_wheel_button_handler((bool)(SET == EXTI_GetEdgeStatus(KEY4_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE)));
	}	
	EXTI_ClearEdgeFlag(KEY4_BUTTON_EXTI_CHANNEL);
}

void EXTI4_15_IRQHandler(void)
{	
	//key3 is the bottom button to change DPI setting
	if (EXTI_GetEdgeFlag(KEY3_BUTTON_EXTI_CHANNEL))
	{		
		if (EXTI_GetEdgeStatus(KEY3_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE))
		{
			option_button_down_handler();
		}
		if (EXTI_GetEdgeStatus(KEY3_BUTTON_EXTI_CHANNEL, EXTI_EDGE_POSITIVE))
		{
			option_button_up_handler();
		}
	}
	EXTI_ClearEdgeFlag(KEY3_BUTTON_EXTI_CHANNEL);
	
	//KEY6 = encoder A (chan 14)
	if (EXTI_GetEdgeFlag(KEY6_BUTTON_EXTI_CHANNEL))
	{		
		encoder_transition_handler();
	}
	EXTI_ClearEdgeFlag(KEY6_BUTTON_EXTI_CHANNEL);
	
	//KEY7 = encoder B (chan 15)
	if (EXTI_GetEdgeFlag(KEY7_BUTTON_EXTI_CHANNEL))
	{		
		encoder_transition_handler();
	}
	EXTI_ClearEdgeFlag(KEY7_BUTTON_EXTI_CHANNEL);
	
	//KEY 8 = side button 1 (chan 10)
	if (EXTI_GetEdgeFlag(KEY8_BUTTON_EXTI_CHANNEL)) //key5 is pb2
	{		
		side_button_1_handler((bool)(SET == EXTI_GetEdgeStatus(KEY8_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE)));
	}	
	EXTI_ClearEdgeFlag(KEY8_BUTTON_EXTI_CHANNEL);
	
	//KEY 9 = side button 1 (chan 8)
	if (EXTI_GetEdgeFlag(KEY9_BUTTON_EXTI_CHANNEL)) //key5 is pb2
	{		
		side_button_2_handler((bool)(SET == EXTI_GetEdgeStatus(KEY9_BUTTON_EXTI_CHANNEL, EXTI_EDGE_NEGATIVE)));
	}	
	EXTI_ClearEdgeFlag(KEY9_BUTTON_EXTI_CHANNEL);
}
