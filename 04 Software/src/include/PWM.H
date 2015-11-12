#ifndef PWM_H
#define PWM_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "AppCfg.h"
/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
#define PWM_MODULE_COUNT_UP		(_PTDIR == 0)
#define PWM_MODULE_COUNT_DOWN	(_PTDIR == 1)

#define PWM_FREQUENCY_kHz		(PWM_FREQUENCY_Hz / 1000.0)			// 3.906 kHz
#define PWM_PERIOD_IN_Tcy		(U16)(cFCY_HZ / PWM_FREQUENCY_Hz)		// 8192
#define PWM_PERIOD				(PWM_PERIOD_IN_Tcy * cTcy)			// 0.000256 s			
#define PWM_PERIOD_us			(1000.0 / PWM_FREQUENCY_kHz)		// 256us
#define PWM_INT_us				(PWM_PERIOD_us / 2.0)				// 128 us
#define PWM_PERIOD_ns			((U32)PWM_PERIOD_us * 1000.0)		// 256000us

#define PWM_DEAD_TIME_us			(4u)							// 4us
#define PWM_DEAD_TIME_ns			(U32)(PWM_DEAD_TIME_us * 1000ul)		// 4000 ns
#define PWM_DEAD_TIME_PRESCALER		(4u)
#define PWM_DEAD_TIME_IN_Tcy		(U16)(PWM_DEAD_TIME_ns / cTcy_ns )

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
#define PU_FAULT()				_FLTAIF
#define PU_FAULT_CLEAR()		_FLTAIF = 0
/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void IF_PWM_Init(void);
void PWM_Output_Test(void);


#endif /* PWM_H */
