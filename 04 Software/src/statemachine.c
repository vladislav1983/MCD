/******************************************************************************/
/* 
 *                      STATE MACHINE IMPLEMENTATION
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef STATEMACHINE_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define STATE_MACHINE_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "statemachine.h"
#include "measure.h"
#include "Control_OpenLoop.h"

/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/
// Commands to statemachine timer
#define READ	0
#define LOAD	1
#define COUNT	2
	
// Statemachine defined times in ms	
#define	c10ms		(10u)
#define	c100ms		(100u)
#define	c1000ms		(1000u)
#define	c2000ms		(2000u)
#define	c5000ms		(5000u)
#define	c10000ms	(10000u)

/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/
	  	 		AppST_States appstate;
static volatile AppST_States app_next_state;
static volatile AppST_States app_prev_state;

/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Exported data to other modules                                             */
/*----------------------------------------------------------------------------*/
volatile U16 SYS_CONTROL;
volatile U16 PC_CONTROL;
volatile U16 ERR_CONTROL;

/*----------------------------------------------------------------------------*/
/* Exported data from other modules                                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static U16 stm_timer_ms(U16 cmd, U16 ms);

/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
* Purpose: 
* Input: 
* Output:  
*/
/******************************************************************************/
void statemachine_contol(void)
{
	_RESET_ERROR_PC_HANDLER(); // Get status of pc control and if is set reset faults

	if(_power_up_ok() && _error_check())
	{
		if( _GET_ERRORS() && (appstate != APP_STATE_ERROR_STATE) )
		{
			DRIVE_DISABLE();	// Drive Disable
			PC_CONTROL = 0;		// Clear All PC Commands
			
			app_prev_state = appstate;  				// Store App State
			appstate       = APP_STATE_ERROR_STATE;		// Go to Error State
		}
		else
		{
			/* Handle High Priority State Machine Transil's here */
			/* These Transil's don't care about statemachine state except errors !!! */
			
			
		
		}
	}
	switch(appstate)
	/*-----------------------------------------------------------------------*/
	{	
	case APP_STATE_INIT:
		
		DRIVE_DISABLE();
	
		stm_timer_ms(LOAD, c2000ms);
		
		appstate 		= APP_STATE_WAIT_STATE_I;
		app_next_state 	= APP_STATE_WAIT_POWER_UP;

	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_WAIT_POWER_UP:
		
		_set_power_up(1);
	
		stm_timer_ms(LOAD, c2000ms);
		
		appstate 		= APP_STATE_WAIT_STATE_I;
		app_next_state 	= APP_ANA_CHANNELS_DC_AUTOTUNNING;

	break;
	/*-----------------------------------------------------------------------*/
	case APP_ANA_CHANNELS_DC_AUTOTUNNING:
		
		if(ANA_DC_Autotunning() == S_OK)
		{
			stm_timer_ms(LOAD, c100ms);
			
			appstate 		= APP_STATE_WAIT_STATE_I;
			app_next_state 	= APP_RESET_ERRORS;
		}

	break;
	/*-----------------------------------------------------------------------*/
	case APP_RESET_ERRORS:
		
		_RESET_ERROR_INTERNAL(); 	// Reset Faults
		
		stm_timer_ms(LOAD, c100ms);
		
		appstate 		= APP_STATE_WAIT_STATE_I;
		app_next_state 	= APP_CHECK_ERRORS;

	break;

	/*-----------------------------------------------------------------------*/
	case APP_CHECK_ERRORS:
		
		if(( _GET_ERRORS() == 0) && (!_power_unit_fault()) )
		{	
			_set_error_check(1);
			
			appstate = APP_WAIT_PC_COMMAND;
		}
		else
		{
			appstate = APP_STATE_ERROR_STATE;
		}
	break;

	/*-----------------------------------------------------------------------*/
	case APP_IDENT:
		
		
		

		
	break;
	/*-----------------------------------------------------------------------*/
	case APP_WAIT_PC_COMMAND:
		
		
		if( _three_phase_gen_control() )
		{
			appstate = APP_STATE_VOLT_FREQ_CONTROL;
			_set_drive_enable_PC(0);
			break;
		}
		if( _test_mode_dio() )
		{
			appstate = APP_STATE_TEST_MODES;
			RESET_DIO_TEST_REG();
			break;
		}
		
	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_VOLT_FREQ_CONTROL:
	
		if( _drive_enable_PC() )
		{
			DRIVE_ENABLE();
		}
		else
		{
			DRIVE_DISABLE();
		}
		
		Voltage_Freq_Control( _get_vf_control_in(), _get_vf_control(), _get_smes(), _drive_enabled());
		
		if( !_three_phase_gen_control() )
		{
			appstate = APP_WAIT_PC_COMMAND;
			DRIVE_DISABLE();
			_set_drive_enable_PC(0);
		}
		
	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_OPEN_LOOP_CONTROL:
		
		
		

		
	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_TEST_MODES:
		
		if( _test_mode_dio() )
		{
			DIO_Test();
		}

	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_ERROR_STATE:
		if( _GET_ERRORS() == 0 ) // if no errors
		{
			app_next_state = app_prev_state;  	// Resturn to state where error was occured 
			
			stm_timer_ms(LOAD, c5000ms);		// Load Timer
			appstate = APP_STATE_WAIT_STATE_I;	// Go to wait state
		}
	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_WAIT_STATE_I:
		stm_timer_ms(COUNT, 0);  // count stm timer
		
		if(!stm_timer_ms(READ, 0)) appstate = app_next_state;

	break;
	/*-----------------------------------------------------------------------*/
	case APP_STATE_WAIT_STATE_II:
		
		
		

		
	break;
	/*-----------------------------------------------------------------------*/

	}// end switch(appstate)

}// END FUNCTION
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*
 * Name: Software Timer for state machine module  TASK1 
 * Params: 
 * cmd: 0 read, 1 start, 2 count 
 * cons: count time in task periods
 * Purpose:   
 * 
 *  cmd: 0 read, 1 start, 2 count 
 *  cons: count time in task periods
 */
/******************************************************************************/

	
static U16 stm_timer_ms(U16 cmd, U16 ms)       		
{    
	static U16 count;

	if(cmd==1)
	{
		count = (U32)ms * 1000ul / (U16)T1_PER_us;
	}	
	else if(count && cmd==2) count--;
	return(count);
}
