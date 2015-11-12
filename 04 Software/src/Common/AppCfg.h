/******************************************************************************/
/* 							APPLICATION CONFIGURATION
 *                             
 *       
 */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifndef APP_CFG_H
#define APP_CFG_H

/* !!! TO CORRECT VIEW TAB SIZE MUST BE 4 SPACES !!! */
/*----------------------------------------------------------------------------*/
/* CPU SPEED DEFINITION                                                       */
/*----------------------------------------------------------------------------*/
//PLL=16x, OSC=8Mhz
#define cOSC_KHz    ((U32)8000.0) 							//oscillator frequancy
#define cPLL_KHz    ((U32)16.0*((cOSC_KHz)))           		// PLL frequency kHz
#define cPLL_MHz    ((U32)(cPLL_KHz)/(U32)1000.0)        	// PLL frequency Mhz
#define cFCY_HZ     (((U32)cPLL_KHz*1000.0)/(U32)4.0)  		// Instruction clock - Hz
#define cFCY_MHZ    ((cPLL_KHz)/(U32)4.0/(U32)1000.0)	  	// Instruction clock - MHz
#define cTcy_ns     ((f32)1000.0/cFCY_MHZ)	  				// Instruction cycle - ns
#define cTcy		(1.0 / cFCY_HZ)

/*----------------------------------------------------------------------------*/
/* SYSTEM CONFIGURATION                                                       */
/*----------------------------------------------------------------------------*/
#define PWM_FREQUENCY_Hz			3906.0

/*----------------------------------------------------------------------------*/
/* VERSION INFORMATION                                                        */
/*----------------------------------------------------------------------------*/ 
#define FIRMWARE_BASE_VERSION		1000
#define FIRMWARE_SUB_VERSION		10

/*----------------------------------------------------------------------------*/
/* INTERRUPT PRIORIRY LEVELS                                                  */
/*----------------------------------------------------------------------------*/ 
/* INTERRUPT PRIORITY LEVELS */
#define cISR_PRIORITY_PWM   		(6u)    /* PWM 		 */     /* DISPATCHER */
#define cISR_PRIORITY_INT1			(5u)	/* INT1 	 */		/* TASK1 */
#define cISR_PRIORITY_FLTA			(4u)	/* FAULT PIN */
#define cISR_PRIORITY_INT2			(3u)	/* INT2 	 */		/* TASK2 */

#define cISR_PRIORITY_U1RX			(3u)	/* UART RX	*/

#define cISR_PRIORITY_TMR1  		(0u)    /* Timer1 	*/      /* DISABLED */
#define cISR_PRIORITY_TMR2			(0u)	/* Timer2 	*/		/* DISABLED */
#define cISR_PRIORITY_TMR3			(0u)	/* Timer3 	*/		/* DISABLED */
#define cISR_PRIORITY_ADC			(0u)	/* ADC   	*/		/* DISABLED */
#define cISR_PRIORITY_U1TX			(0u)	/* UART TX  */		/* DISABLED */


/*----------------------------------------------------------------------------*/
/* GLOBAL DEFINES                                                             */
/*----------------------------------------------------------------------------*/ 
#define dsPIC30F6015_EEPROM_CONFIGURED	/* dsPIC30F6015 EEPROM ADDRES CHECKED AND SET */
#define DEAD_TIME_COMP_EN				/* Dead Time Compensation Enabled */

#endif /* APPCFG_H */
