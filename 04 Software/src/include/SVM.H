#ifndef SVM_H
#define SVM_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "Measure.h"
/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define UMAX				(4096ul)	// Normalized maximum voltage vector (= 2*Udc/3)
#define PDC_MAX_IN_Tcy		(PWM_PERIOD_IN_Tcy * 2.0)

#define T_MIN_ns			(3000u + PWM_DEAD_TIME_ns)
#define T_MIN_IN_Tcy		(U16)(T_MIN_ns / cTcy_ns )

#define T_MAX_IN_Tcy		(PWM_PERIOD_IN_Tcy - PWM_DEAD_TIME_IN_Tcy)
/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
typedef struct sSVM_data
{
	S16 t0;	
	S16 tr;
	S16 tl;

}tsSVM_data;

typedef struct sPWM_Times
{
	U16 T_u_on;
	U16 T_v_on;
	U16 T_w_on;
	
	U16 T_u_off;
	U16 T_v_off;
	U16 T_w_off;
	
}tsPWM_Times;

extern U16 PWM_CONTROL_REG;
/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
extern 	tsSVM_data 						svpwm;
#define _get_svpwm()			    	(&svpwm)

extern tsPWM_Times 						pwmtimes;
#define _get_pwmtimes()			    	(&pwmtimes)



/* PWM_CONTROL_REG  */
/*	
	PWM_CONTROL_REG  
	Bit 0		-	Phase U Current Sign 	1: Negative 0: Positive	
	Bit 1		-	Phase V Current Sign 	1: Negative 0: Positive	
	Bit 2		-	Phase W Current Sign 	1: Negative 0: Positive	
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
#define _get_sign_Iu()					(_getbit(PWM_CONTROL_REG,0))  	 /* bit 0 */
#define _set_get_sign_Iu(x)				(_putbit((x),PWM_CONTROL_REG,0)) /* bit 0 */

#define _get_sign_Iv()					(_getbit(PWM_CONTROL_REG,1))  	 /* bit 1 */
#define _set_get_sign_Iv(x)				(_putbit((x),PWM_CONTROL_REG,1)) /* bit 1 */

#define _get_sign_Iw()					(_getbit(PWM_CONTROL_REG,2))  	 /* bit 2 */
#define _set_get_sign_Iw(x)				(_putbit((x),PWM_CONTROL_REG,2)) /* bit 2 */




/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void SVM_GEN( S16 qVd, S16 qVq, U16 qAlpha_ref );
void Load_Ton(void);
void Load_Toff(void);
void Get_Current_Signs(tsMeasure_Struct *ms);


#endif /* SVM_H */
