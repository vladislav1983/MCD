#ifndef STATEMACHINE_H
#define STATEMACHINE_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "basedef.h"

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
extern volatile U16 SYS_CONTROL;
extern volatile U16 PC_CONTROL;
extern volatile U16 ERR_CONTROL;






/* Application State Machine States */

typedef enum teAppST_States
{
	/*0*/  APP_STATE_INIT 			= 0,
	/*1*/  APP_STATE_WAIT_POWER_UP,
	/*2*/  APP_RESET_ERRORS,
	/*3*/  APP_CHECK_ERRORS,
	/*4*/  APP_ANA_CHANNELS_DC_AUTOTUNNING,
	/*5*/  APP_IDENT,
	/*6*/  APP_WAIT_PC_COMMAND,
	/*7*/  APP_STATE_VOLT_FREQ_CONTROL,
	/*8*/  APP_STATE_OPEN_LOOP_CONTROL,
	/*9*/  APP_STATE_TEST_MODES,
	/*10*/ APP_STATE_ERROR_STATE,
	/*11*/ APP_STATE_WAIT_STATE_I,
	/*12*/ APP_STATE_WAIT_STATE_II,
}AppST_States;

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
/* SYSTEM CONTROL  */
/*	
	SYS_CONTROL 
	Bit 0		-	Power UP OK 			1: Power Up OK 				0: Power Up Fail	
	Bit 1		-	Drive Enable			1: Drive ENABLED  			0: Drive DISABLED	
	Bit 2		-	Error Check				1: Check Errors				0: Do Not Check Errors	
	Bit 3		-	IDENT Attempts			1: IDENT Second Attempt		0: IDENT first attempt
	Bit 4		-	
	Bit 5		-	
	Bit 6		-	
	Bit 7		-	
	Bit 8		-	
	Bit 9		-	
	Bit 10		-	
	Bit 11		-	
	Bit 12		-	
	Bit 13		-	
	Bit 14		-	
	Bit 15		-	
*/
#define _power_up_ok()					(_getbit(SYS_CONTROL,0))  	 /* bit 0 */
#define _set_power_up(x)				(_putbit((x),SYS_CONTROL,0)) /* bit 0 */

#define _drive_enabled()				(_getbit(SYS_CONTROL,1))  	 /* bit 1 */
#define _set_drive_enabled(x)			(_putbit((x),SYS_CONTROL,1)) /* bit 1 */

#define _error_check()					(_getbit(SYS_CONTROL,2))  	 /* bit 2 */
#define _set_error_check(x)				(_putbit((x),SYS_CONTROL,2)) /* bit 2 */

#define _IDENT_second_attempt()			(_getbit(SYS_CONTROL,3))  	 /* bit 3 */
#define _set_IDENT_second_attempt(x)	(_putbit((x),SYS_CONTROL,3)) /* bit 3 */

//#define _()							(_getbit(SYS_CONTROL,4))  	 /* bit 4 */
//#define _(x)							(_putbit((x),SYS_CONTROL,4)) /* bit 4 */

//#define _()							(_getbit(SYS_CONTROL,5))  	 /* bit 5 */
//#define _(x)							(_putbit((x),SYS_CONTROL,5)) /* bit 5 */

//#define _()							(_getbit(SYS_CONTROL,6))  	 /* bit 6 */
//#define _(x)							(_putbit((x),SYS_CONTROL,6)) /* bit 6 */

//#define _()							(_getbit(SYS_CONTROL,7))  	 /* bit 7 */
//#define _(x)							(_putbit((x),SYS_CONTROL,7)) /* bit 7 */

//#define _()							(_getbit(SYS_CONTROL,8))  	 /* bit 8 */
//#define _(x)							(_putbit((x),SYS_CONTROL,8)) /* bit 8 */

//#define _()							(_getbit(SYS_CONTROL,9))  	 /* bit 9 */
//#define _(x)							(_putbit((x),SYS_CONTROL,9)) /* bit 9 */

//#define _()							(_getbit(SYS_CONTROL,10))  	 /* bit 10 */
//#define _(x)							(_putbit((x),SYS_CONTROL,10)) /* bit 10 */

//#define _()							(_getbit(SYS_CONTROL,11))  	 /* bit 11 */
//#define _(x)							(_putbit((x),SYS_CONTROL,11)) /* bit 11 */

//#define _()							(_getbit(SYS_CONTROL,12))  	 /* bit 12 */
//#define _(x)							(_putbit((x),SYS_CONTROL,12)) /* bit 12 */

//#define _()							(_getbit(SYS_CONTROL,13))  	 /* bit 13 */
//#define _(x)							(_putbit((x),SYS_CONTROL,13)) /* bit 13 */

//#define _()							(_getbit(SYS_CONTROL,14))  	 /* bit 14 */
//#define _(x)							(_putbit((x),SYS_CONTROL,14)) /* bit 14 */

//#define _()							(_getbit(SYS_CONTROL,15))  	 /* bit 15 */
//#define _(x)							(_putbit((x),SYS_CONTROL,15)) /* bit 15 */

/*	
	PC_CONTROL 
	Bit 0		-	Reset error register ERR_CONTROL 	1: Reset  		0: Do not reset
	Bit 1		-	DIO_Test							1: DIO Test		0: None	
	Bit 2		-	PWM_OVERRIDE TEST					1: PWM_TEST		0: None
	Bit 3		-	
	Bit 4		-	
	Bit 5		-	
	Bit 6		-	
	Bit 7		-	
	Bit 8		-
	Bit 9		-
	Bit 10		-
	Bit 11		-
	Bit 12		-
	Bit 13		-
	Bit 14		-	
	Bit 15		-	
*/

/* PC CONTROL FLAGS */
#define _reset_error_reg()					(_getbit(PC_CONTROL,0))     /* bit 0 */
#define _set_reset_error_reg(x)				(_putbit((x),PC_CONTROL,0)) /* bit 0 */

#define _test_mode_dio()					(_getbit(PC_CONTROL,1))     /* bit 1 */
#define _set_test_mode_dio(x)				(_putbit((x),PC_CONTROL,1)) /* bit 1 */

#define _three_phase_gen_control()			(_getbit(PC_CONTROL,2))     /* bit 2 */
#define _set_three_phase_gen_control(x)		(_putbit((x),PC_CONTROL,2)) /* bit 2 */

#define _pwm_override_test()				(_getbit(PC_CONTROL,3))     /* bit 3 */
#define _set_pwm_override_test(x)			(_putbit((x),PC_CONTROL,3)) /* bit 3 */

#define _drive_enable_PC()					(_getbit(PC_CONTROL,4))     /* bit 4 */
#define _set_drive_enable_PC(x)				(_putbit((x),PC_CONTROL,4)) /* bit 4 */



/*	
	ERR_CONTROL 
	Bit 0		-	
	Bit 1		-	
	Bit 2		-	
	Bit 3		-	
	Bit 4		-	
	Bit 5		-	
	Bit 6		-	
	Bit 7		-	
	Bit 8		-		
	Bit 9		-	
	Bit 10		-	
	Bit 11		-
	Bit 12		-
	Bit 13		-
	Bit 14		-
*/
/* ERROR CONTROL MACROS */
#define _GET_ERRORS()					(ERR_CONTROL | EE_ERR_REG)

/* Reset Error Register and clear external power unit fault latch */
#define _RESET_ERROR_INTERNAL()			ERR_CONTROL = 0;	\
										PU_Fault_Reset()		

/* Reset error register macro */
#define _RESET_ERROR_PC_HANDLER()(	\
{									\
	if(_reset_error_reg())			\
	{								\
		ERR_CONTROL = 0;			\
		_set_reset_error_reg(0);	\
		PU_Fault_Reset();			\
	}								\
})

#define _inverter_overcurrent_fault()					(_getbit(ERR_CONTROL,0))  	 /* bit 0 */
#define _set_inverter_overcurrent_fault(x)				(_putbit((x),ERR_CONTROL,0)) /* bit 0 */

#define _inverter_overvoltage_fault()					(_getbit(ERR_CONTROL,1))  	 /* bit 1 */
#define _set_inverter_overvoltage_fault(x)				(_putbit((x),ERR_CONTROL,1)) /* bit 1 */

#define _dc_link_overcurrent_fault()					(_getbit(ERR_CONTROL,2))  	 /* bit 2 */
#define _set_dc_link_overcurrent_fault(x)				(_putbit((x),ERR_CONTROL,2)) /* bit 2 */

#define _dc_link_overvoltage_fault()					(_getbit(ERR_CONTROL,3))  	 /* bit 3 */
#define _set_dc_link_overvoltage_fault(x)				(_putbit((x),ERR_CONTROL,3)) /* bit 3 */

#define _dc_link_undervoltage_fault()					(_getbit(ERR_CONTROL,4))  	 /* bit 4 */
#define _set_dc_link_undervoltage_fault(x)				(_putbit((x),ERR_CONTROL,4)) /* bit 4 */

#define _ana_channels_offset_fault()					(_getbit(ERR_CONTROL,5))  	 /* bit 5 */
#define _set_ana_channels_offset_fault(x)				(_putbit((x),ERR_CONTROL,5)) /* bit 5 */

#define _power_unit_fault()								(_getbit(ERR_CONTROL,6))  	 /* bit 6 */
#define _set_power_unit_fault(x)						(_putbit((x),ERR_CONTROL,6)) /* bit 6 */

#define _task1_runtime_fault()							(_getbit(ERR_CONTROL,7))  	 /* bit 7 */
#define _set_task1_runtime_fault(x)						(_putbit((x),ERR_CONTROL,7)) /* bit 7 */

#define _task2_runtime_fault()							(_getbit(ERR_CONTROL,8))  	 /* bit 8 */
#define _set_task2_runtime_fault(x)						(_putbit((x),ERR_CONTROL,8)) /* bit 8 */

//#define _()				(_getbit(ERR_CONTROL,8))  	 /* bit 8 */
//#define _(x)				(_putbit((x),ERR_CONTROL,8)) /* bit 8 */

//#define _()				(_getbit(ERR_CONTROL,9))  	 /* bit 9 */
//#define _(x)				(_putbit((x),ERR_CONTROL,9)) /* bit 9 */

//#define _()				(_getbit(ERR_CONTROL,10))  	  /* bit 10 */
//#define _(x)				(_putbit((x),ERR_CONTROL,10)) /* bit 10 */

//#define _()				(_getbit(ERR_CONTROL,11))  	  /* bit 11 */
//#define _(x)				(_putbit((x),ERR_CONTROL,11)) /* bit 11 */

//#define _()				(_getbit(ERR_CONTROL,12))  	  /* bit 12 */
//#define _(x)				(_putbit((x),ERR_CONTROL,12)) /* bit 12 */

//#define _()				(_getbit(ERR_CONTROL,13))  	  /* bit 13 */
//#define _(x)				(_putbit((x),ERR_CONTROL,13)) /* bit 13 */

//#define _()				(_getbit(ERR_CONTROL,14))  	  /* bit 14 */
//#define _(x)				(_putbit((x),ERR_CONTROL,14)) /* bit 14 */

//#define _()				(_getbit(ERR_CONTROL,15))  	  /* bit 15 */
//#define _(x)				(_putbit((x),ERR_CONTROL,15)) /* bit 15 */

/*----------------------------------------------------------------------------*/
/* Exported functions          					           				      */
/*----------------------------------------------------------------------------*/
void statemachine_contol(void);
#endif /* STATEMACHINE_H */
