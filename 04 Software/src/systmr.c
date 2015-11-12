/******************************************************************************/
/* 
 *            				SYSTEM TIMER INTEGRATION                 
 *       
 */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef SYSTMR_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define SYSTMR_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include <libpic30.h>
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

/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/


/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
* Purpose: Timer 1 Setup. Task 1 RunTime Measure.
* Input: none
* Output: none
*/
/******************************************************************************/
void IF_SysTmr1_Init(void)
{
   
	T1CONbits.TSIDL = TMR1_STOP_IN_IDLE_MODE;
	T1CONbits.TGATE = TMR1_GATED_TIME_ACUMULATION_ENABLE;
	T1CONbits.TCKPS = TMR1_PRESCALER;
	T1CONbits.TCS 	= TMR1_CLOCK_SOURCE;

    TMR1 = 0;
    PR1 = 65535;	// set period1 register
	
    _T1IP = cISR_PRIORITY_TMR1;     		// set priority level
    _T1IF = 0;                      		// clear interrupt flag
    _T1IE = 0;                      		// enable interrupts

    T1CONbits.TON = 1;              		// start the timer
}
/******************************************************************************/
/*
* Purpose: Timer 3 Setup. Drive Task 2
* Input: none
* Output: none
* Note : Priority --> Below TASK1
*/
/******************************************************************************/
void IF_SysTmr3_Init(void)
{

    T3CONbits.TSIDL = TMR3_STOP_IN_IDLE_MODE;
	T3CONbits.TGATE = TMR3_GATED_TIME_ACUMULATION_ENABLE;
	T3CONbits.TCKPS = TMR3_PRESCALER;
	T3CONbits.TCS 	= TMR3_CLOCK_SOURCE;
	
	TMR3 = 0;
    PR3 = (U16)((((cFCY_MHZ) * (cTMR3_ISR_TICK_us)) / (cTMR3_PRESC)) - 1ul);	 // set period1 register
	
    _T3IP = cISR_PRIORITY_TMR3;     		// set priority level
    _T3IF = 0;                      		// clear interrupt flag
    _T3IE = 1;                      		// enable interrupts

    T3CONbits.TON = 1;              		// start the timer
}

/******************************************************************************/
/*
* Purpose: Timer 2 Setup. Measure Task 2 RunTime.
* Input: none
* Output: none
* Priority 0
*/
/******************************************************************************/
void IF_SysTmr2_Init(void)
{

    T2CONbits.TSIDL = TMR2_STOP_IN_IDLE_MODE;
	T2CONbits.TGATE = TMR2_GATED_TIME_ACUMULATION_ENABLE;
	T2CONbits.TCKPS = TMR2_PRESCALER;
	T2CONbits.T32 	= TMR2_T32;
	T2CONbits.TCS 	= TMR2_CLOCK_SOURCE;
	
	PR2 = 65535;
	
	TMR2 = 0;	
    _T2IP = cISR_PRIORITY_TMR2;     		// set priority level
    _T2IF = 0;                      		// clear interrupt flag
    _T2IE = 0;                      		// disable interrupts

    T2CONbits.TON = 1;              		// start the timer
}

/******************************************************************************/
/*
* Purpose: Delay in microseconds.
* Input: none
* Output: none
*/
/******************************************************************************/
void delay_us(U32 u32us)
{		
	U32 u32ticks;
	
	u32ticks = ( ((U32)u32us * 1000)  / cTcy_ns);
	
	__delay32(u32ticks);
}

/******************************************************************************/
/*
* Purpose: Delay in miliseconds.
* Input: none
* Output: none
*/
/******************************************************************************/
void delay_ms(U32 u32ms)
{
	U32 u32ticks;
	
	u32ticks = ( ((U32)u32ms * 1000)  / cTcy_ns) * 1000;
	
	__delay32(u32ticks);
}

