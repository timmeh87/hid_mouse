/* Includes ------------------------------------------------------------------------------------------------*/


#include "ht32_board_config.h"
#include "ht32_usbd_core.h"
#include "ht32_usbd_class.h"
#include "ht32_usbd_descriptor.h"
#include "pixart.h"
#include "mouse.h"

//firmware
#include "firmware.c"

extern MouseSystem_t SYSTEM;

static void pixart_cs_high(void)
{
	my_GPIOOn(GPIO_PA, GPIO_PIN_3); 
}
static void pixart_cs_low(void)
{
	my_GPIOOff(GPIO_PA, GPIO_PIN_3); 
}
//write only one byte. takes ~50us to write but it needs a long guard time after too
// so if guard_time is true then it takes ~220us to complete
static void pixart_write1(u8 address, u8 data, bool guard_time)
{
	//write procedure:	
	// set ncs low
	// wait 120ns
	// clock out the address and then the data (will take 4us*2 at 2mhz)
	// hold the line for 35us from the last clock edge
	// set ncs high
	// !: last bit of next write has to be 180us later
	// !: first bit of next read DATA has to be 180us later
	// so just have a 180us back porch for safety
		
	pixart_cs_low(); //initiate by pulling cs low
	//120ns delay (6 clocks @ 48mhz)
	__asm		("nop;nop;nop;nop;nop"); 	//provide 5 clocks of delay after CS transition, just in case the SendData call was inlined
	address |= 0x80;						//at least one cycle is used right here too, so 6 cycles of delay
	SPI_SendData(HTCFG_MASTER_SPI, address);	//send write
	SPI_SendData(HTCFG_MASTER_SPI, data);	
	Delay(2);									//min 35us then, cs high again
	pixart_cs_high();
	
	if(guard_time) Delay(9);					//9 ticks = 180us, max time to wait for subsequent op
}

//reads only one byte. takes ~200us to complete
static u8 pixart_read1(u8 address)
{
	//read procedure:
	// set ncs low
	// wait 120ns
	// clock out address
	// wait a full 160us
	// clock out data
	// wait 120ns
	// cs high
	// !: first bit of next command MUST be 20us later
	
	
	u8 data;
	pixart_cs_low();
	//120ns delay (6 clocks @ 48mhz)
	__asm		("nop;nop;nop;nop"); 	//provide 4 clocks of delay incase the SendData call was inlined
	address &= 0x7f;
	SYSTEM.spiReceivedCount=0;
	SPI_SendData(HTCFG_MASTER_SPI, address);
	Delay(8); //min 160us
	SPI_SendData(HTCFG_MASTER_SPI, 0xff);
	Delay(1); //needed for interrupt to fire
	data = SYSTEM.spiReceiveBuffer[1];
	pixart_cs_high();
	Delay(1); //20us delay beofore any other command
	return data;
}


bool InitPixartSensor(void)
{
/* 
	1. apply power
	2. drive CS high, then low
	3. write 0x5A to power_up_register, wait 50ms
	4. do a dummy read of registers 2-6 
	5. download SROM
		1. write x00 to x10
		2. write x1d to to x13
		3. wait 10ms
		4. write x18 to x13
		5. write entire firmware 4095 bytes
	
	6. write address 0x3d with data 0x80
	7. poll address 0x3d now looking for 0xc0 (MANDATORY 1ms interval! 55ms timeout)
	8. clear address 0x3d and then write address 0x10 with data 0x20
	9. do your own init */
	
	//1 ____
	// 		|_____ reset
	my_GPIOOn(GPIO_PB, GPIO_PIN_5); //deassert reset
	Delay(2800); //50 ms = 20us * 2500
	
	//2       ____
	//  _____|    |_____ CS (20us high)
	Delay(1);
	pixart_cs_low();
	Delay(1);
	pixart_cs_high();
	Delay(1);
	pixart_cs_low();
	
	Delay(10);
	
	//3
	pixart_write1(0x3a, 0x5a, true);
	Delay(2800);
	
	//4
	u8 dat;	
	for (int x =0; x<0x7; x++)
	{
		dat = pixart_read1(x);	
		Delay(1);
	}
	
	//5
		/*1. write x00 to x10
		2. write x1d to to x13
		3. wait 10ms
		4. write x18 to x13
		5. write entire firmware 4095 bytes*/
	pixart_write1(PX_Config2, 0x00, true); //disable rest
	pixart_write1(PX_SROM_Enable, 0x1d, true);
	Delay(650); //10ms = 20us * 500
	pixart_write1(PX_SROM_Enable, 0x18, true);

	pixart_cs_low();
	for (int x=0; x<firmware_v6_length; x++)
	{
		Delay(1); //min 15us
		SPI_SendData(HTCFG_MASTER_SPI, firmware_v6_data[x]);
	}
	
	Delay(10); //min 15us to CS toggle
	pixart_cs_high();
	Delay(2); //min 1 us
	pixart_cs_low();
	
	Delay(12); //min 200us delay to read id
	if(pixart_read1(PX_SROM_ID) != 0x6) return false;
	
	
	//7
	Delay(650); //10ms = 20us * 500
	pixart_write1(0x3d, 0x80, true); //undocumented register?
	int c = 55;
	dat=0;
	uint32_t start_time = get_systick();
	while (c>0)
	{		
		dat = pixart_read1(0x3d);
		while(get_systick() - start_time < 50){}; //50 ticks is 1ms
		start_time = get_systick();
		if(dat==0xc0) break;
		c--;
	}
	//for me, c counts down to about 39 before dat==0xc0
	pixart_write1(0x3d, 0x00, true); //undocumented register?
	pixart_write1(PX_Config2, 0x00, true); //enable rest
	
	return false;
	
	
	/*
	//observation can be cleared then checked. only one bit to indicate srom running
	pixart_write1(PX_Observation, 0x0);
	Delay(10);
	pixart_read1(PX_Observation, &dat);
	Delay(1);
	Delay(12); //min 200us delay to read id
	pixart_read1(PX_SROM_ID, &dat);
	
	//get detailed checkum.. not sure how to verify this crc
	pixart_write1(PX_SROM_Enable, 0x15);
	Delay(10);
	u8 h, l;
	pixart_read1(PX_Data_Out_Lower, &h);
	Delay(1);
	pixart_read1(PX_Data_Out_Upper, &l);
	*/
	
}

//read the data from the mouse using its "motion burst" feature which overall is faster for multiple bytes
//however there is a long setup time so its barely faster. This function absolutely cannot take longer than the report period which is 1000us. 
// under 500us is good to leave time for other tasks. 
motion_burst_t pixart_motion_burst(void)
{
	motion_burst_t ret = {0};	
	pixart_write1(PX_Motion_Burst, 0xaa, true); //write any value to motion burst (with guard (guard seems required, not clear in datasheet) - 200us)	
	pixart_cs_low();
	SPI_SendData(HTCFG_MASTER_SPI, PX_Motion_Burst); //then send motion burst address (4us)
	Delay(2); //require at least 35us;											      (40us)
	SYSTEM.spiReceivedCount = 0;
	
	//note: there are 12 bytes to read but we only read the first 6 for speed, the rest are not important.
	for (int x=0; x<6; x++)
	{
		SPI_SendData(HTCFG_MASTER_SPI, 0x00);
		Delay(1);	//not necessary for SPI timing but we need time for the interrupt to fire		(24x6 = ~150u)
	}
	pixart_cs_high();
	
	for (int x=0; x<6; x++)
	{
		((u8*)&ret)[x] = SYSTEM.spiReceiveBuffer[x];
	}
	
	// total = 200+20+4+40+150 = 414us
	return ret;
}

void pixart_set_cpi(u16 cpi)
{
	int value = cpi/50;
	pixart_write1(PX_Resolution_H, 0, true);
	pixart_write1(PX_Resolution_L, value, true);	
}

void pixart_set_liftoff(u8 liftoff)
{
	if ((liftoff == 2) || (liftoff == 3))  //only two valid values
	{				
		pixart_write1(PX_Lift_Config, liftoff, true);
		SYSTEM.set_liftoff=liftoff;
	}
}
void pixart_set_threshold(u8 threshold)
{
	if (threshold)
		pixart_write1(PX_Raw_Data_Threshold, threshold, true);	
}
void pixart_set_squal(u8 squal)
{
	if (squal)
		pixart_write1(PX_Min_SQ_Run, squal, true);
}

static bool last_calibration_succeeded = false;
void pixart_liftoff_cal_begin(void)
{
	pixart_write1(PX_LiftCutoff_Tune1, 0x80, true);	
}

//returns true if it is still running, or false if it is done
bool pixart_liftoff_cal_running(void)
{
/*	Poll CAL_STAT[2:0] of LiftCutoff_Cal1 to check the status of the calibration procedure. There are three ways to
	successfully stop the calibration procedure: set RUN_CAL register bit to 0 if either:
	a. CAL_STAT[2:0] of LiftCutoff_Cal1 register = 0x02,
	b. CAL_STAT[2:0] of LiftCutoff_Cal1 register = 0x02 & user initiates a stop through a mouse-click event, or,
	c. CAL_STAT[2:0] of LiftCutoff_Cal1 register = 0x03.
	If CAL_STAT[2:0] of LiftCutoff_Cal1 register = 0x04, the calibration procedure needs to be re-started*/
		
	u8 status;
	status = pixart_read1(PX_LiftCutoff_Tune1) & 0x07;
	switch (status)
	{
		case 0: return false; //idle
		case 1: return true;  //in progress
		case 2: 				//succeed on distance requirement, but continuing
		{
			last_calibration_succeeded = true;
			pixart_write1(PX_LiftCutoff_Tune1, 0x02, true);	//stop
			Delay(50); //wait for result			
		}
		break;
		case 3: 				//succeed and timed out
		{
			last_calibration_succeeded = true;
			pixart_write1(PX_LiftCutoff_Tune1, 0x03, true);	//stop
			Delay(50); //wait for result
		}
		break;
		case 4: 				//fail
		{
			last_calibration_succeeded = false;
			pixart_write1(PX_LiftCutoff_Tune1, 0x04, true);	//stop
		}
		break;
	} 
	return false;
}

bool pixart_liftoff_cal_save(void)
{
	if (last_calibration_succeeded == true)
	{
		u8 squal_setting = pixart_read1(PX_LiftCutoff_Tune2);
		u8 thresh_setting = pixart_read1(PX_LiftCutoff_Tune3) & 0x7f;
		
		SaveMouseSettings_t settings = LoadSettings();
		settings.SQUAL_cal = squal_setting;
		settings.threshold_cal = thresh_setting;
		SaveSettings(settings);
		
		//also have to actually set the registers now
		pixart_set_squal(settings.SQUAL_cal);	
		pixart_set_threshold(settings.SQUAL_cal);
		return true;		
	}
	return false;
}
