/******************************************************************************/
/* 						DIGITAL IO IMPLEMENTATION
 *                             
 *       
 */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef DIGITALIO_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define DIGITALIO_C
#endif

/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"


/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/  

/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/
// Commands to timer
#define READ	0
#define LOAD	1
#define COUNT	2

// Times defined
#define	c10ms		(10u)
#define	c100ms		(100u)
#define	c200ms		(200u)
#define	c1000ms		(1000u)
#define	c2000ms		(2000u)
#define	c5000ms		(5000u)
#define	c10000ms	(10000u)
/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
U16 DIO_TEST_REG;


/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static U16 user_led_timer_ms(U16 cmd, U16 ms);

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*
* Purpose: Digital IO Init
* Input: none
* Output: none
*/
/******************************************************************************/
void IF_DigitalIO_Init(void)
{   
	/* Power Unit Control and Fault Reset pins Init */
	pinDRV_EN = 0; 			pinDRV_EN_dir = 0;			// output,0
	pinPU_FAULT_RST = 1; 	pinPU_FAULT_RST_dir = 0;	// output,1
	
	/* User LED Pins Init */
	pinERROR_LED = 0;		pinERROR_LED_dir = 0;			// output,0
	pinSTATUS_LED = 0;		pinSTATUS_LED_dir = 0;			// output,0
	pinPUF_LED = 0;			pinPUF_LED_dir = 0;			// output,0
	pinSTATE_LED = 0;		pinSTATE_LED_dir = 0;			// output,0
	
	/* User Buttons Pins Init */
	
	pinBUT1_dir = 1;		// Input
	pinBUT2_dir = 1;		// Input
	pinBUT3_dir = 1;		// Input
	pinBUT4_dir = 1;		// Input
	
	user_led_timer_ms(LOAD, c100ms); // Init user led timer with LED base tick
	
	//pinPU_FAULT_dir = 1;		// Input
}

/******************************************************************************/
/*
* Purpose: Power Unit Fault Reset 
* Input: none
* Output: none
*/
/******************************************************************************/
void PU_Fault_Reset(void)
{
	
	pinPU_FAULT_RST = 0; // Set pin low. 
	
	delay_us(5);
		
	pinPU_FAULT_RST = 1;
	
	delay_us(5);
	
	pinPU_FAULT_RST = 0; // Set pin low second pulse. We must be sure that external latch is cleared
	
	delay_us(5);
	
	pinPU_FAULT_RST = 1; // Set pin high (default state)
		
}
/******************************************************************************/
/*
* Purpose: Control User Led's
* Input: none
* Output: none
*/
/******************************************************************************/
void User_LEDS_Control(void)
{
	
	user_led_timer_ms(COUNT, 0);  // count timer
	
	if( !user_led_timer_ms(READ, 0) )
	{
		if( _GET_ERRORS() == 0 )
		{
			// If NO ERRORS blink status led (green)
			pinSTATUS_LED = ~pinSTATUS_LED;
			
			
			pinPUF_LED    = 0;
			pinERROR_LED = 0; // Turn Off Error Led
		}
		else
		{
			pinSTATUS_LED = 0; // Turn Off Status LED
			
			// If ERROR Present blink ERROR LED (RED)
			pinERROR_LED = ~pinERROR_LED;
			
			if( _power_unit_fault() )
			{
				// If Power Unit Fault - blink PUF led (red)
				pinPUF_LED = ~pinPUF_LED;
			}
		
		}
		
		user_led_timer_ms(LOAD, c100ms); // Init user led timer with LED base tick	
	
	}

}

/******************************************************************************/
/*
* Purpose: DIO Test
* Input: none
* Output: none
*/
/******************************************************************************/
void DIO_Test(void)
{
	
	if(_PC_PORTF_5())
	{
		pinERROR_LED = 1;
	}
	else
	{
		pinERROR_LED = 0;
	}
	
	if(_PC_PORTF_4())
	{
		pinSTATUS_LED = 1;
	}
	else
	{
		pinSTATUS_LED = 0;
	}
	
	if(_PC_PORTF_1())
	{
		pinPUF_LED = 1;
	}
	else
	{
		pinPUF_LED = 0;
	}
	
	if( _PC_PORTF_0() )
	{
		pinSTATE_LED = 1;
	}
	else
	{
		pinSTATE_LED = 0;
	}
	
	if( _PC_PORTD_0() )
	{
		pinPU_FAULT_RST = 1;
	}
	else
	{
		pinPU_FAULT_RST = 0;
	}
	
	if( _PC_PORTD_1() )
	{
		pinDRV_EN = 1;
	}
	else
	{
		pinDRV_EN = 0;
	}
	
}

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
static U16 user_led_timer_ms(U16 cmd, U16 ms)       		
{    
	static U16 count;

	if(cmd==1)
	{
		count = (U32)ms * 1000ul / (U16)T1_PER_us;
	}	
	else if(count && cmd==2) count--;
	return(count);
}

