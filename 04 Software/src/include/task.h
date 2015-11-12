#ifndef TASK_H
#define TASK_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "systmr.h"

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define T2_PERIOD			(U16)(T2_PER_us/PWM_INT_us)		// Task 2 Period In PWM Periods

#define T1_PER_us			(S16)(PWM_PERIOD_us)
#define T2_PER_us			(5000u)
#define fT1_PERIOD			(PWM_PERIOD)




#define STACK_BASE_ADDRESS	0x800
/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

struct sTaskCounterStruct
{
	U16 T1_count;
	U16 T2_count;
};

struct sTaskRuntineParStruct
{
	// Task's Ticks
	U16 Task1Ticks;
	U16 Task2Ticks;
	U16 Task1RunTicks;
	U16 Task2RunTicks;
	
	// Task's Runtimes
	U32 Task1Time_us;
	U32 Task2Time_us;
	U32 Task1RunTime_us;
	U32 Task2RunTime_us;
	
	// Stack Sizes
	U16 Stack_Size;	
	U16 Stack_Free;
	U16 Stack_Base;
	
	// Task's Stack usage
	U16 Task1_Stack_Usage;
	U16 Task2_Stack_Usage;
};
/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
extern struct sTaskRuntineParStruct 		sTimes;
#define _get_sTimes()						(&sTimes)

extern struct sTaskCounterStruct			task_count;
#define _get_task_count()					(&task_count)


#define _Task1_Timer_Clear()		(TMR1 = 0)
#define _Task1_Timer_Stop()			(T1CONbits.TON = 0)
#define _Task1_Timer_Start()		(T1CONbits.TON = 1)
#define _Task1_GetTime()			(sTimes.Task1Ticks = TMR1)
#define _Task1_GetRunTime()			(sTimes.Task1RunTicks = TMR1 - sTimes.Task1Ticks)

#define _Task2_Timer_Clear()		(TMR2 = 0)
#define _Task2_Timer_Stop()			(T2CONbits.TON = 0)
#define _Task2_Timer_Start()		(T2CONbits.TON = 1)
#define _Task2_GetTime()			(sTimes.Task2Ticks = TMR2)
#define _Task2_GetRunTime()			(sTimes.Task2RunTicks = TMR2 - sTimes.Task2Ticks)


extern U16 TASK_CONTROL;
/*	
	TASK CONTROL
	Bit 0		-	task1 execution status 			1:Task execution in progress,		0:Task is not execute
	Bit 1		-	task2 execution status 			1:Task execution in progress,		0:Task is not execute
	Bit 2		-		
	Bit 3		-		
	Bit 4		-		
	Bit 5		-		
	Bit 6		-		
	Bit 7		-		
*/
#define _is_task1_execute()  		(_getbit(TASK_CONTROL,0))     	/* bit 0 */
#define _set_task1_execute(x)  		(_putbit((x),TASK_CONTROL,0))   /* bit 0 */

#define _is_task2_execute()  		(_getbit(TASK_CONTROL,1))     	/* bit 1 */
#define _set_task2_execute(x)  		(_putbit((x),TASK_CONTROL,1))   /* bit 1 */


/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void TaskTimesCalc(struct sTaskRuntineParStruct *p);
void Task1_Stack_Usage_Calc(struct sTaskRuntineParStruct *p, bool init);
void Task_Init(void);


#endif /* TASK_H */
