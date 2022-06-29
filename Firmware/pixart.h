#ifndef __PIXART_H
#define __PIXART_H
#include "ht32.h"

#define PX_Product_ID  0x00
#define PX_Revision_ID 0x01
#define PX_Motion  0x02
#define PX_Delta_X_L 0x03
#define PX_Delta_X_H 0x04
#define PX_Delta_Y_L 0x05
#define PX_Delta_Y_H 0x06
#define PX_SQUAL 0x07
#define PX_Raw_Data_Sum  0x08
#define PX_Maximum_Raw_data  0x09
#define PX_Minimum_Raw_data  0x0A
#define PX_Shutter_Lower 0x0B
#define PX_Shutter_Upper 0x0C
#define PX_Control 0x0D
#define PX_Resolution_L 0x0e
#define PX_Resolution_H 0x0F
#define PX_Config2 0x10
#define PX_Angle_Tune  0x11
#define PX_Frame_Capture 0x12
#define PX_SROM_Enable 0x13
#define PX_Run_Downshift 0x14
#define PX_Rest1_Rate_Lower  0x15
#define PX_Rest1_Rate_Upper  0x16
#define PX_Rest1_Downshift 0x17
#define PX_Rest2_Rate_Lower  0x18
#define PX_Rest2_Rate_Upper  0x19
#define PX_Rest2_Downshift 0x1A
#define PX_Rest3_Rate_Lower  0x1B
#define PX_Rest3_Rate_Upper  0x1C
#define PX_Observation 0x24
#define PX_Data_Out_Lower  0x25
#define PX_Data_Out_Upper  0x26
#define PX_Raw_Data_Dump 0x29
#define PX_SROM_ID 0x2A
#define PX_Min_SQ_Run  0x2B
#define PX_Raw_Data_Threshold  0x2C
#define PX_Config5 0x2F
#define PX_Power_Up_Reset  0x3A
#define PX_Shutdown  0x3B
#define PX_Inverse_Product_ID  0x3F
#define PX_LiftCutoff_Tune3  0x41
#define PX_Angle_Snap  0x42
#define PX_LiftCutoff_Tune1  0x4A
#define PX_Motion_Burst  0x50
#define PX_SROM_Load_Burst 0x62
#define PX_Lift_Config 0x63
#define PX_Raw_Data_Burst  0x64
#define PX_LiftCutoff_Tune2  0x65
#define PX_LiftCutoff_Tune_Timeout 0x71
#define PX_LiftCutoff_Tune_Min_Length  0x72
#define PX_PWM_Period_Count 0x73
#define PX_PWM_Width_Count  0x74

//this is the data that is returned by a motion burst, in order
typedef __PACKED_H struct 
{
	u8 motion;
	u8 observation;
	s16 dx;
	s16 dy;
	u8 SQAL;
	u8 rawdata_sum;
	u8 max_rawdata;
	u8 min_rawdata;
	u8 shut_up;
	u8 shut_low;
}motion_burst_t;

bool InitPixartSensor(void);
motion_burst_t pixart_motion_burst(void);
void pixart_set_threshold(u8 threshold);
void pixart_set_squal(u8 squal);
void pixart_set_cpi(u16 cpi);
void pixart_set_liftoff(u8 liftoff);

void pixart_liftoff_cal_begin(void);
bool pixart_liftoff_cal_save(void);
bool pixart_liftoff_cal_running(void);

#endif
