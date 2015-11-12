/******************************************************************************/
/* 
 *                   PID CONTROLLER
 *       			 V 1.00
 *                   V.Gyurov 20.10.2010
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef VPID_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define VPID
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "vPID.h"
/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/
// Pid Controller Delay Line Indexes
#define n						(0)	// n
#define n1						(1)	// n-1
#define n2						(2)	// n-2

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
/* Exported data to other modules                                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data from other modules                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/


/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
* Purpose: PID Controller 
* Input:   Pointer to PID structure
* Output:  PID Control Output
* 
* Operation:
*
*                                             ----   Proportional
*                                            |    |  Output
*                             ---------------| Kp |-----------------
*                            |               |    |                 |
*                            |                ----                  |
*Reference                   |                                     ---
*Input         ---           |           --------------  Integral | + | Control   -------
*     --------| + |  Control |          |      Ki      | Output   |   | Output   |       |
*             |   |----------|----------| ------------ |----------|+  |----------| Plant |--
*        -----| - |Difference|          |  1 - Z^(-1)  |          |   |          |       |  |
*       |      ---  (error)  |           --------------           | + |           -------   |
*       |                    |                                     ---                      |
*       | Measured           |         -------------------  Deriv   |                       |
*       | Outut              |        |                   | Output  |                       |
*       |                     --------| Kd * (1 - Z^(-1)) |---------                        |
*       |                             |                   |                                 |
*       |                              -------------------                                  |
*       |                                                                                   |
*       |                                                                                   |
*        -----------------------------------------------------------------------------------
*
*   vPID_Output[n] = vPID_Output[n-1]
*                    + controlHistory[n]   * abcCoefficients[0]
*                    + controlHistory[n-1] * abcCoefficients[1]
*                    + controlHistory[n-2] * abcCoefficients[2]
*
*  where:
*   abcCoefficients[0] = Kp + Ki + Kd
*   abcCoefficients[1] = -(Kp + 2*Kd)
*   abcCoefficients[2] = Kd
*   controlHistory[n]  = vPID_Fbk[n] - vPID_Ref[n]
*  where:
*   abcCoefficients, controlHistory, controlOutput, measuredOutput and controlReference
*   are all members of the data structure tPID.
*   n   = n0 = 0
*   n-1 = n1 = 1
*   n-2 = n2 = 2
*
* Input:
*       Address of tPID data structure
*
* Return:
*       PID Output
*/
/******************************************************************************/
S16 vPID(tsvPID *pid)
{
	S32 s32control_error;
	S32 s32tmp;
	
	s32control_error = (pid)->vPID_Ref - (pid)->vPID_Fbk;
	
	// Calculate Control Error
	(pid)->controlHistory[n] = (S16)(Hi(s32control_error));
	
	// Calculate cannonical form of pid controller equation
	// vPID_Output[n] = vPID_Output[n-1]
    //                + controlHistory[n]   * abcCoefficients[0]
    //                + controlHistory[n-1] * abcCoefficients[1]
    //                + controlHistory[n-2] * abcCoefficients[2]
	s32tmp = (pid)->vPID_Output;											 //   vPID_Output[n-1]
	s32tmp += _fmul( (pid)->abcCoefficients[0], (pid)->controlHistory[n] );  // + controlHistory[n]   * abcCoefficients[0]
	s32tmp += _fmul( (pid)->abcCoefficients[1], (pid)->controlHistory[n1] ); // + controlHistory[n-1] * abcCoefficients[1]
	s32tmp += _fmul( (pid)->abcCoefficients[2], (pid)->controlHistory[n2] ); // + controlHistory[n-2] * abcCoefficients[2]

	(pid)->vPID_Output = (S16)(Hi(s32tmp));	// Store rounded result
	
	//Update the error history on the delay line
	(pid)->controlHistory[n2] = (pid)->controlHistory[n1];
	(pid)->controlHistory[n1] = (pid)->controlHistory[n];

	return( (pid)->vPID_Output);
}

/******************************************************************************/
/*
* Purpose: Init PID Controller
* Input: pointer to PID structure
* Output: none 
*/
/******************************************************************************/
void vPID_Init(tsvPID *pid)
{
	(pid)->vPID_Ref = 0;
	(pid)->vPID_Fbk = 0;
	
	(pid)->controlHistory[0] = 0;
	(pid)->controlHistory[1] = 0;
	(pid)->controlHistory[2] = 0;
	
	(pid)->vPID_Output = 0;
}

/******************************************************************************/
/*
* Purpose: PID Controller Coefficients Calculation
* Input: pointer to PID structure
* Output: none 
*/
/******************************************************************************/
void vPID_Coeff_Calc(tsvPID *pid)
{
	S32 s32tmp;
	
	s32tmp = (pid)->qKp + (pid)->qKi + (pid)->qKd;
	
	(pid)->abcCoefficients[0] = (S16)(Hi(s32tmp));
	
	s32tmp = -((pid)->qKp + (2*(pid)->qKd));
	
	(pid)->abcCoefficients[1] = (S16)(Hi(s32tmp));
	
	(pid)->abcCoefficients[2] = (pid)->qKd;

}
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/


