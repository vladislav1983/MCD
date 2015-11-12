/******************************************************************************/
/* 
 *                            SPACE VECTOR MODULATION
 *                              V.Gyurov 17.10.2010          
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef SVM_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define SVM_C 
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "SVM.H"
#include "Measure.h"

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
/* Exported data to other modules                                             */
/*----------------------------------------------------------------------------*/
tsSVM_data  svpwm;

tsPWM_Times pwmtimes;

U16 PWM_CONTROL_REG;
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
* Purpose: Space Vector Modulation
* Input: 
* Output:  
*/
/******************************************************************************/
void SVM_GEN( S16 qVd, S16 qVq, U16 qAlpha_ref)
{
	S16 qGamma;
	S16 umax;
	S16 Vs;
	U16 sector;
	S16 t_sum;
	tsSVM_data  *p;
	tsPWM_Times *pio;
	
	p   = _get_svpwm();
	pio = _get_pwmtimes();
		
	/******************************************************************
	*
	*  qGamma = Resultant Vector in dq coordinate system between Vd and Vq
	*
	*******************************************************************/
	qGamma  = ATAN( qVq, qVd );
	qAlpha_ref += qGamma;
	
	/******************************************************************
	*
	*  Resultant Voltage Vector Calculation
	*  Vs = SQRT(Vd^2 + Vq^2)
	*
	*******************************************************************/
	Vs = sqrt_32( (U32)qVd*qVd + (U32)qVq*qVq );
	
	/******************************************************************
	*
	*  Resultant Voltage Vector Limitation
	*  0 <= Vs <= UMAX
	*
	*******************************************************************/
	if( Vs > UMAX ) Vs = UMAX; 
	if( Vs < 0 )    Vs = 0;
	
	/******************************************************************
	*
	*  Sector Estimation. Scale down qAlpha_ref to 0<= qAlpha_ref <= 60.
	*  1 <= sector <= 6
	*
	*******************************************************************/
	for(sector = 1; qAlpha_ref > (U16)(60ul*65536ul/360ul + 1); sector++)
	{
		qAlpha_ref -= (U16)(60ul*65536ul/360ul + 1);
	}
	
	/******************************************************************
	*
	*  Voltage Limit Calculation
	*  umax = UMAX*sqrt(3)/(2*cos(30deg-alpha))
	*
	*******************************************************************/
	umax = _div_32( (UMAX*qSQRT3/2), cosine((U16)(30ul*65536ul/360ul) - qAlpha_ref), _UDIV );
	if( umax > UMAX ) umax = UMAX;
	if (Vs > umax) Vs = umax;
	
	/******************************************************************
	*
	*  Switching Time Calculation for Dummies.
	*  Due particularity of dsPIC PWM module we consider following spectial cases:
	*  1: PWM_PERIOD_IN_Tcy = PTPER * 2.0 -> This is done because LSB of PDCx registers is used for match
	*  of PWM event - therefore the max value written to PDCx register is two times of PTPER register (PTPER * 2.0).
	*  This must be seen in t0 calculation.
	*
	*  And what about t_sum calculation ??? See the second case.
	*  2: t_sum is resultant vector amplitude expressed in switching times. 
	*  Here switching period is considered to be (PWM_PERIOD_IN_Tcy * 2), which actually is PTPER register.
	*  V.G. 18.10.2010
	*******************************************************************/
	
	t_sum = _div_32( ((U32)Vs * ((U32)PWM_PERIOD_IN_Tcy  << 1)), (SQRT3*UMAX/2), _SDIV);
	(p)->tr = _mul_16( t_sum, sine((U16)(60ul*65536ul/360ul + 1) - qAlpha_ref) );
	(p)->tl = _mul_16( t_sum, sine(qAlpha_ref) );
	
	(p)->t0 = ( (S16)PWM_PERIOD_IN_Tcy - (p)->tr - (p)->tl) >> 1;
	
	switch(sector)
	{
	/******************************************************************
	* SECTOR 1
	*******************************************************************/
		case 1:
			#ifdef DEAD_TIME_COMP_EN
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase U
			*******************************************************************/
			if(!_get_sign_Iu())	// If Current is positive
			{
				(pio)->T_u_off = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_u_on = (pio)->T_u_off + (S16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_u_on = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_u_off = (pio)->T_u_on + (S16)PWM_DEAD_TIME_IN_Tcy;	
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase V
			*******************************************************************/
			if(!_get_sign_Iv())		// If Current is positive
			{
				(pio)->T_v_off = (p)->tl + (p)->t0;
				(pio)->T_v_on = (pio)->T_v_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else		// If Current is negative
			{
				(pio)->T_v_on = (p)->tl + (p)->t0;
				(pio)->T_v_off = (pio)->T_v_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase W
			*******************************************************************/			
			if(!_get_sign_Iw())		// If Current is positive
			{
				(pio)->T_w_off = (p)->t0;
				(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else		// If Current is negative
			{
				(pio)->T_w_on = (p)->t0;
				(pio)->T_w_off = (pio)->T_w_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			#else
			/******************************************************************
			* Switching Times Calculation without dead time compensation
			*******************************************************************/
			(pio)->T_u_off = (p)->tr + (p)->tl + (p)->t0;
			(pio)->T_u_on = (pio)->T_u_off;
			
			(pio)->T_v_off = (p)->tl + (p)->t0;
			(pio)->T_v_on = (pio)->T_v_off;
			
			(pio)->T_w_off = (p)->t0;
			(pio)->T_w_on = (pio)->T_w_off;
			#endif
			
			
		break;
	/******************************************************************
	* SECTOR 2
	*******************************************************************/
		case 2:
			#ifdef DEAD_TIME_COMP_EN
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase U
			*******************************************************************/
			if(!_get_sign_Iu())	// If Current is positive
			{
				(pio)->T_u_off = (p)->tr + (p)->t0;
				(pio)->T_u_on = (pio)->T_u_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_u_on = (p)->tr + (p)->t0;
				(pio)->T_u_off = (pio)->T_u_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase V
			*******************************************************************/
			if(!_get_sign_Iv())		// If Current is positive
			{
				(pio)->T_v_off = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_v_on = (pio)->T_v_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else		// If Current is negative
			{
				(pio)->T_v_on = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_v_off = (pio)->T_v_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase W
			*******************************************************************/			
			if(!_get_sign_Iw())		// If Current is positive
			{
				(pio)->T_w_off = (p)->t0;
				(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else		// If Current is negative
			{
				(pio)->T_w_on = (p)->t0;
				(pio)->T_w_off = (pio)->T_w_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			#else
			/******************************************************************
			* Switching Times Calculation without dead time compensation
			*******************************************************************/
			(pio)->T_u_off = (p)->tr + (p)->t0;
			(pio)->T_u_on = (pio)->T_u_off;	
			
			(pio)->T_v_off = (p)->tr + (p)->tl + (p)->t0;
			(pio)->T_v_on = (pio)->T_v_off;
			
			(pio)->T_w_off = (p)->t0;
			(pio)->T_w_on = (pio)->T_w_off;
			#endif				
		break;
	/******************************************************************
	* SECTOR 3
	*******************************************************************/
		case 3:
			#ifdef DEAD_TIME_COMP_EN
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase U
			*******************************************************************/
			if(!_get_sign_Iu())	// If Current is positive
			{
				(pio)->T_u_off = (p)->t0;
				(pio)->T_u_on = (pio)->T_u_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_u_on = (p)->t0;
				(pio)->T_u_off = (pio)->T_u_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase V
			*******************************************************************/
			if(!_get_sign_Iv())	// If Current is positive
			{
				(pio)->T_v_off = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_v_on = (pio)->T_v_off + (U16)PWM_DEAD_TIME_IN_Tcy;
				
			}
			else	// If Current is negative
			{
				(pio)->T_v_on = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_v_off = (pio)->T_v_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase W
			*******************************************************************/			
			if(!_get_sign_Iw())	// If Current is positive
			{
				(pio)->T_w_off = (p)->tl + (p)->t0;
				(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_w_on = (p)->tl + (p)->t0;
				(pio)->T_w_off = (pio)->T_w_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			#else
			/******************************************************************
			* Switching Times Calculation without dead time compensation
			*******************************************************************/
			(pio)->T_u_off = (p)->t0;
			(pio)->T_u_on = (pio)->T_u_off;
			
			(pio)->T_v_off = (p)->tr + (p)->tl + (p)->t0;
			(pio)->T_v_on = (pio)->T_v_off;
			
			(pio)->T_w_off = (p)->tl + (p)->t0;
			(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			#endif
		break;
	/******************************************************************
	* SECTOR 4
	*******************************************************************/
		case 4:
			#ifdef DEAD_TIME_COMP_EN
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase U
			*******************************************************************/
			if(!_get_sign_Iu())	// If Current is positive
			{
				(pio)->T_u_off = (p)->t0;
				(pio)->T_u_on = (pio)->T_u_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_u_on = (p)->t0;
				(pio)->T_u_off = (pio)->T_u_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase V
			*******************************************************************/
			if(!_get_sign_Iv())	// If Current is positive
			{
				(pio)->T_v_off = (p)->tr + (p)->t0;
				(pio)->T_v_on = (pio)->T_v_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_v_on = (p)->tr + (p)->t0;
				(pio)->T_v_off = (pio)->T_v_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase W
			*******************************************************************/			
			if(!_get_sign_Iw())	// If Current is positive
			{
				(pio)->T_w_off = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;		
			}
			else	// If Current is negative
			{
				(pio)->T_w_on = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_w_off = (pio)->T_w_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			#else
			/******************************************************************
			* Switching Times Calculation without dead time compensation
			*******************************************************************/
			(pio)->T_u_off = (p)->t0;
			(pio)->T_u_on = (pio)->T_u_off;
			
			(pio)->T_v_off = (p)->tr + (p)->t0;
			(pio)->T_v_on = (pio)->T_v_off;
			
			(pio)->T_w_off = (p)->tr + (p)->tl + (p)->t0;
			(pio)->T_w_on = (pio)->T_w_off;
			#endif
		break;
	/******************************************************************
	* SECTOR 5
	*******************************************************************/
		case 5:
			#ifdef DEAD_TIME_COMP_EN
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase U
			*******************************************************************/
			if(!_get_sign_Iu())	// If Current is positive
			{
				(pio)->T_u_off = (p)->tl + (p)->t0;
				(pio)->T_u_on = (pio)->T_u_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_u_on = (p)->tl + (p)->t0;
				(pio)->T_u_off = (pio)->T_u_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase V
			*******************************************************************/
			if(!_get_sign_Iv())	// If Current is positive
			{
				(pio)->T_v_off = (p)->t0;
				(pio)->T_v_on = (pio)->T_v_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_v_on = (p)->t0;
				(pio)->T_v_off = (pio)->T_v_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase W
			*******************************************************************/			
			if(!_get_sign_Iw())	// If Current is positive
			{
				(pio)->T_w_off = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;	
			}
			else	// If Current is negative
			{
				(pio)->T_w_on = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_w_off = (pio)->T_w_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			#else
			/******************************************************************
			* Switching Times Calculation without dead time compensation
			*******************************************************************/
			(pio)->T_u_off = (p)->tl + (p)->t0;
			(pio)->T_u_on = (pio)->T_u_off;
			
			(pio)->T_v_off = (p)->t0;
			(pio)->T_v_on = (pio)->T_v_off;
			
			(pio)->T_w_off = (p)->tr + (p)->tl + (p)->t0;
			(pio)->T_w_on = (pio)->T_w_off;
			#endif
			
		break;
	/******************************************************************
	* SECTOR 6
	*******************************************************************/
		case 6:
			#ifdef DEAD_TIME_COMP_EN
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase U
			*******************************************************************/
			if(!_get_sign_Iu())	// If Current is positive
			{
				(pio)->T_u_off = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_u_on = (pio)->T_u_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_u_on = (p)->tr + (p)->tl + (p)->t0;
				(pio)->T_u_off = (pio)->T_u_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase V
			*******************************************************************/
			if(!_get_sign_Iv())	// If Current is positive
			{
				(pio)->T_v_off = (p)->t0;
				(pio)->T_v_on = (pio)->T_v_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_v_on = (p)->t0;
				(pio)->T_v_off = (pio)->T_v_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			
			/******************************************************************
			* Switching Times Calculation and Dead Time Compensation for phase W
			*******************************************************************/			
			if(!_get_sign_Iw())	// If Current is positive
			{
				(pio)->T_w_off = (p)->tr + (p)->t0;
				(pio)->T_w_on = (pio)->T_w_off + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			else	// If Current is negative
			{
				(pio)->T_w_on = (p)->tr + (p)->t0;
				(pio)->T_w_off = (pio)->T_w_on + (U16)PWM_DEAD_TIME_IN_Tcy;
			}
			#else
			/******************************************************************
			* Switching Times Calculation without dead time compensation
			*******************************************************************/
			(pio)->T_u_off = (p)->tr + (p)->tl + (p)->t0;
			(pio)->T_u_on = (pio)->T_u_off;
			
			(pio)->T_v_off = (p)->t0;
			(pio)->T_v_on = (pio)->T_v_off;
			
			(pio)->T_w_off = (p)->tr + (p)->t0;
			(pio)->T_w_on = (pio)->T_w_off;
			
			#endif
			
		break;
		/*-----------------------------------------------------------*/
		default:
			Nop();
		break;

	}// end switch	
	
}// END FUNCTION

/******************************************************************************/
/*
* Purpose: Load T_on
* Input: none
* Output: none 
* Note: Load Transistor OFF times. This is done in PWM interrupt when PWM COUNTING UP
*/
/******************************************************************************/
void Load_Ton(void)
{
	tsPWM_Times *pio;
	
	pio = _get_pwmtimes();
	
	/******************************************************************
	* Switching On Time Limitation for Phase U
	*******************************************************************/
	if( (pio)->T_u_on > T_MIN_IN_Tcy )
	{
		if( (pio)->T_u_on < T_MAX_IN_Tcy ) PDC1 = (pio)->T_u_on;
		else PDC1 = T_MAX_IN_Tcy;	// Limit to max switching time
	}
	else
	{
		PDC1 = T_MIN_IN_Tcy;		// Limit to min switching time
	}
	
	/******************************************************************
	* Switching On Time Limitation for Phase V
	*******************************************************************/
	if( (pio)->T_v_on > T_MIN_IN_Tcy )
	{
		if( (pio)->T_v_on < T_MAX_IN_Tcy ) PDC2 = (pio)->T_v_on;
		else PDC2 = T_MAX_IN_Tcy;	// Limit to max switching time
	}
	else
	{
		PDC2 = T_MIN_IN_Tcy;		// Limit to min switching time
	}
	
	/******************************************************************
	* Switching On Time Limitation for Phase W
	*******************************************************************/
	if( (pio)->T_w_on > T_MIN_IN_Tcy )
	{
		if( (pio)->T_w_on < T_MAX_IN_Tcy ) PDC3 = (pio)->T_w_on;
		else PDC3 = T_MAX_IN_Tcy;	// Limit to max switching time
	}
	else
	{
		PDC3 = T_MIN_IN_Tcy;		// Limit to min switching time
	}

}

/******************************************************************************/
/*
* Purpose: Load T_off
* Input: none
* Output: none 
* Note: Load Transistor ON times. This is done in PWM interrupt when PWM COUNTING DOWN
*/
/******************************************************************************/
void Load_Toff(void)
{
	tsPWM_Times *pio;
	
	pio = _get_pwmtimes();
	
	/******************************************************************
	* Switching Off Time Limitation for Phase U
	*******************************************************************/
	if( (pio)->T_u_off > T_MIN_IN_Tcy )
	{
		if( (pio)->T_u_off < T_MAX_IN_Tcy ) PDC1 = (pio)->T_u_off;
		else PDC1 = T_MAX_IN_Tcy;	// Limit to max switching time
	}
	else
	{
		PDC1 = T_MIN_IN_Tcy;		// Limit to min switching time
	}
	/******************************************************************
	* Switching Off Time Limitation for Phase V
	*******************************************************************/
	if( (pio)->T_v_off > T_MIN_IN_Tcy )
	{
		if( (pio)->T_v_off < T_MAX_IN_Tcy ) PDC2 = (pio)->T_v_off;
		else PDC2 = T_MAX_IN_Tcy;	// Limit to max switching time
	}
	else
	{
		PDC2 = T_MIN_IN_Tcy;		// Limit to min switching time
	}
	
	/******************************************************************
	* Switching Off Time Limitation for Phase W
	*******************************************************************/
	if( (pio)->T_w_off > T_MIN_IN_Tcy )
	{
		if( (pio)->T_w_off < T_MAX_IN_Tcy ) PDC3 = (pio)->T_w_off;
		else PDC3 = T_MAX_IN_Tcy;	// Limit to max switching time
	}
	else
	{
		PDC3 = T_MIN_IN_Tcy;		// Limit to min switching time
	}

}
/******************************************************************************/
/*
* Purpose: Get Sign Of Inverter Currents
* Input: Measure Structure - Access of currents feedbacks
* Output: none 
* Note: Get Signs of currents. Used for dead time compensation
*/
/******************************************************************************/
void Get_Current_Signs(tsMeasure_Struct *ms)
{
	
	if( (ms)->qIu > 0) _set_get_sign_Iu(0);
	else _set_get_sign_Iu(1);
	
	if( (ms)->qIv > 0) _set_get_sign_Iv(0);
	else _set_get_sign_Iv(1);
	
	if( (ms)->qIw > 0) _set_get_sign_Iw(0);
	else _set_get_sign_Iw(1);

}
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/
