#ifndef __HT32_MOUSE_H
#define __HT32_MOUSE_H
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "ht32_usbd_core.h"
#include "ht32_usbd_class.h"
#include "ht32_usbd_descriptor.h"
#include "pixart.h"
#include "production.h"

typedef struct{
	u16 Header;
	u16 TrimValue;
}SaveAutoTrim_t;

typedef __PACKED_H struct{
	u32 Header;
	u32 cpi;
	u32 report_period;
	u32 liftoff;
	u32 SQUAL_cal;
	u32 threshold_cal;
}SaveMouseSettings_t;

typedef __PACKED_H struct
{
	u8 B1 :1;
	u8 B2 :1;
	u8 B3 :1;
	u8 B4 :1;
	u8 B5 :1;
} __PACKED_F mouse_report_button_t;

typedef __PACKED_H union 
{
	u8 Byte;
	mouse_report_button_t bits;
} __PACKED_F mouse_report_button_u;

typedef __PACKED_H struct
{
	mouse_report_button_u Button;
	s8 x;
	s8 y;
	s8 wheel;
} __PACKED_F mouse_report_t;

typedef enum
{
	MOUSE_MODE,
	SETTING_MODE,
	LIFOFF_CAL_ARMED,
	DOING_LIFTOFF_CAL,
	LIFTOFF_FAILED
} settings_states_e;

typedef enum
{
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_WHITE,
	COLOR_BLACK
} colors_e;

typedef enum
{
	CPI_VALUE = 0,
	CPI_LED_BLINKS = 1,
	CPI_LED_COLOR = 2
} cpi_settings_e;

typedef struct{
	u32 usbIsLowPowerAllowed; 

	//clock auto trim stuff
	bool autoTrimSaved;
	u32 autoTrimValue;
	
	settings_states_e settings_state;

	vs16 set_cpi; 			//sets CPI according to an array in main
	vs16 set_report_period; //counts how many SOFs between reports 0 = 1000hz
	vs16 set_liftoff;		// =  2 or 3;

	//these are from SPI
	vs16 spiReceivedCount;// = 0;
	vu8 spiReceiveBuffer[20];
	
	vu32 dpiDownTime;
	
	volatile bool dpiButtonUp;
	volatile bool dpiShortPress;
	volatile bool dpiLongPress;
	
	volatile mouse_report_t MouseNextReport;
	volatile mouse_report_t MouseState; // only used to store buttons from ISR
	
	volatile bool need_update;
}MouseSystem_t;

#define DPI_LONG_PRESS_TICKS (500*50)
//needs a better flash eeprom algorithm but for now, we just keep erasing and writing the first address of a whole page for each of these settings

//note: auto-trim example puts the value in the option section which also includes the flash locking bits and some other stuff
// the example code always erases all of the options bits. kind of annoying
#define SAVE_AUTO_TRIM_ADDRESS            (0x1FF00050)  //0x1FF00050 is somewhere in the option byte rom extra space
#define SAVE_AUTO_TRIM_HEAD_KEY           (0x5A5A)

//settings go into Flash Page 126 0x0001F800 ~ 0x0001FBFF 1 KB
#define SETTINGS_FLASH_PAGE				 (31)
#define SAVE_SETTINGS_ADDRESS            (0x00007c00)  //0x1FF00050 is somewhere in the option byte rom extra space
#define SAVE_SETTINGS_HEAD_KEY           (0x5A5B)

/* main.c function prototypes -----------------------------------------------------------------------------*/
void CKCU_Configuration(void);
void GPIO_Configuration(void);
void USB_Configuration(void);
void USBPLL_Configuration(void);
void USBVRG_Configuration(void);
void CKCU_LoadTrimValue(void);
void UpdateAutoTrimValue(void);
void Suspend(u32 uPara);

void SPI_Configuration(void);

void Delay(u32 nTime);
void Delay_ms(u32 nTime); //ALSO RUNS THE USB ;)

//main system timer, 20us per tick
uint32_t get_systick(void);

//gets the accumulated motion since last calling this function
int8_t get_wheel_motion(void);

//basic gpio
void my_GPIOOn(u32 bGpioId, u32 uGPIOPin);
void my_GPIOOff(u32 bGpioId, u32 uGPIOPin);
void my_LEDOff(void); //top side led, usually not installed. can be used as a general purpose debug output
void my_LEDOn(void);
void my_RGBSet(bool r, bool g, bool b);
void my_RGBSetColor(colors_e color);
void my_RGBBlinkColor(u8 n_blinks, colors_e color1, colors_e color2, u16 period_ms);

//save and load all the mouse settings including liftoff calibration
bool SaveSettings(SaveMouseSettings_t settings);
SaveMouseSettings_t LoadSettings(void);

#endif
