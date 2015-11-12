#ifndef SYSTMR_H
#define SYSTMR_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define CPU_CYCLE_ns							(31.25)

/* TMR1 */
#define cTMR1_PRESC								(0b00)		//timer1 prescaller
#define TMR1_STOP_IN_IDLE_MODE 					0			//Disable
#define TMR1_GATED_TIME_ACUMULATION_ENABLE		0			//Disable
#define TMR1_PRESCALER							(0b00)		//Prescaler = 1
#define TMR1_CLOCK_SOURCE						0			//Internal clock

/* TMR3 */
#define cTMR3_ISR_TICK_us						(5000u)		//timer3 tick in us = task2 time (5ms)
#define cTMR3_PRESC								(8u)
#define TMR3_STOP_IN_IDLE_MODE 					0			//Disable
#define TMR3_GATED_TIME_ACUMULATION_ENABLE		0			//Disable
#define TMR3_PRESCALER							(0b01)		//Prescaler = 8
#define TMR3_CLOCK_SOURCE						0			//Internal clock

/* TMR2 */
#define TMR2_STOP_IN_IDLE_MODE 					0			//Disable
#define TMR2_GATED_TIME_ACUMULATION_ENABLE		0			//Disable
#define TMR2_PRESCALER							(0b01)		//Prescaler = 8
#define TMR2_T32								0
#define TMR2_CLOCK_SOURCE						0
#define TMR2_PRESCALER_INT						8

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported functions                     				      				  */
/*----------------------------------------------------------------------------*/
void IF_SysTmr1_Init(void);
void IF_SysTmr2_Init(void);
void IF_SysTmr3_Init(void);

void delay_ms(U32 u32ms);
void delay_us(U32 u32us);

#endif /* SYSTMR_H */
