/******************************************************************************/
/* 
 *                           IDENTIFICATION
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef IDENT_C 
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define IDENT_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "measure.h"
#include "IDENT.h"
#include "ACIM.h"
#include "SVM.h"

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
#define UBETA_MAX   (S16)(SQRT3*UMAX/2.0)         /* voltage maximum in beta axis */

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
*
*             --------------           --------------
*            |              |         |              |    
*      o-----|      Rs      |---------|  Sigma * Ls  |-------------o-----------------------------        
*            |              |         |              |             |                            |
*      ^      --------------           --------------              |       <--------            |
*      |                                                           |        (Lr / Lm) * Ir      |
*      |                     --------->                         -------                      -------
*      |                         Is                            |       |                    |       |
*      |                                                       |       |                    |       |
*      |                                                       |       |                    |       | 
*      Us                                                      |       | (1-Sigma) * Ls     |       | (Lm^2 / Lr^2) * Rr
*      |                                                |      |       |                    |       |
*      |                                             Im |      |       |                    |       |
*      |                                                |      |       |                    |       |
*      |                                                v       -------                      -------
*      |                                                           |                            |
*      v                                                           |                            |
*                                                                  |                            |                   
*      o-----------------------------------------------------------o-----------------------------                                                                                           
*                                                                                                
*    Ã - eqvivalent circuit of induction motor - Static Model (No Rotating Rotor)
*    Used for offline identification of leakage inductance. Offline method, short pulse transient.
*    
*    Rs - Stator Resistance
*    Ls - Stator Self Inductance
*    Sigma - tolal leakage factor = 1 - (Lm^2 / Ls * Lr)   
*    Lr - Rotor Inductance
*    Rr - Rotor Resistance
*    Lm - Mutual Inductance
*    Is - Stator Current
*    Im - Magnetizing Current                                                              
*/
/******************************************************************************/


/******************************************************************************/
/*
* Purpose: Identification of SigmaLss 
* Input: tsMeasure_Struct poitner
* Output: Voltage vector used for identification
*  0 -> IDLE, Do nothing
* -1 -> ident OK
* -2 -> wrong current direction
* -3 -> wrong sigma Lss identification
*
* Explanation:
* To measure the transient leakage inductance a short voltage impulse is
* applied to the stator winding, and the current gradient is measured. Since
* the time needed for this test pulse is very short, and the process is barely
* noticeable, this measurement can be carried out also outside a special
* identification run. The leakage inductance arises from:
* 			
*            SigmaLss = u_test*T/i.q, note that d component is zero (or close to zero :)).
*
* For an appropriate width of the voltage pulse and measurement over the
* complete current slope a good average inductance value will be obtained.
* 
* Reference: Vector Control of Three-Phase AC Machines
* N.P. Quang, Hanoi University of Technology, Hanoi, Vietnam; J.-A. Dittrich, Zurich, Switzerland
* Chapter 6 - 6.4.2 Current-voltage characteristics of the inverter, stator
* resistance and transient leakage inductance
*/
/******************************************************************************/
S16 Ident_SigmaLss(tsMeasure_Struct *pio)
{
	static S16 ident_state;
	static S16 us_tmp;	// Temporary test voltage vector
	
	S16 us;				// Test voltage vector
	S16 itest;			// Test Current
	
	tsMotPar *mpar;
	struct sInv_Params_Struct *ipar;
	
	mpar = _get_motpar();
	ipar = _get_sInv_Params();
	
	us = 0;	//Clear us - this is important!
	
	
	switch(ident_state)
	{
		/*-----------------------------------------------------------*/
		case 0:
			ident_state = 1;	// delay one task1 period (256us)

		break;
		/*-----------------------------------------------------------*/
		case 1:
			// apply test voltage puls u_test = sigmaLss*SQRT2*IN_mot/(2*T) in beta axis
			us = (S16)(SQRT2*UMAX*10/(T1_PER_us));
			us = _div_32((S32)us*(((mpar)->IN_mot>>1)*(mpar)->SigmaLss),Get_Vphase_max(), _SDIV);
			
			us_tmp = us;	// Store calculated voltage vector
			
			if(us > UBETA_MAX) us = UBETA_MAX;	// Voltage limitation
			
			ident_state = 2;	// Voltage applyed, go to next state
			
		break;
		/*-----------------------------------------------------------*/
		case 2:
			us = 0;
			ident_state = 3;	// Voltage Removed, go to next state
		break;
		/*-----------------------------------------------------------*/
		case 3:
		
			// compare ialpha (i.d) and ibeta (i.q) with reference itest = iN_mot/2
			// id has to be 0, iq has to be equal itest
			
			itest = (S16)(SQRT2*4096/2.0);
			itest = _div_32((S32)itest*(mpar)->IN_mot,(ipar)->Inv_Current_max, _SDIV);
			
			if(us_tmp > UBETA_MAX)
			{
				us = UBETA_MAX;	// Voltage limitation
				
				itest = _div_32((S32)us * itest, us_tmp, _SDIV);	// This is -> (UBETA_MAX * T) / sigmaLss;
			}
			
			if( (absi((pio)->qId) > (itest >> 1) ) || ((pio)->qIq < -(itest >> 1)) )
			{
				//(mpar)->SigmaLss_ID = 0;
				ident_state = -2;	// return Us = -2 -> wrong currents
			}
			else	// currents correct
			{
			   /*
         		*       calculate transient leakage inductance
         		*       sigmaLs' = u_test*T/i.q = sigmaLs*SQRT2*IN_mot/(4*i.q)
         		*/
         		
         		(mpar)->SigmaLss_ID = _div_32((S32)itest * (mpar)->SigmaLss, (pio)->qIq ,_SDIV);
         		
         		if( ((mpar->SigmaLss_ID) > (mpar->SigmaLss >> 2)) && ((mpar->SigmaLss_ID) < (4 * mpar->SigmaLss)) )
         		{
	         		
	         		//if( (mpar)->SigmaLss_ID < (mpar)->SigmaLss ) (mpar)->SigmaLss = (mpar)->SigmaLss_ID;
	         		
	         		ident_state = -1;	// Identification finished successfully
         		}
         		else
         		{
	         		ident_state = -3;	// Wrong sigmaLss identification
         		}	
			}
			
			// If identification error present - attempt second identification after200ms. This is done when state is -2(wrong current direction),
			// or -3 (wrong sigma Lss identification)
			// Time 200ms is used to be sure current is decayed!
			// us_tmp is used as timer. Load 200ms time in us1!
			us_tmp = (S16)((200.0 * 1000) / T1_PER_us);
			us = 0;		// Return 0V voltage
		break;
		/*-----------------------------------------------------------*/
		case -1:
			us = -1;							// Return -1 - identification finished successfully
			_set_IDENT_second_attempt(0);		// Set IDENT attempts to 0
			ident_state = 0;					// Return to init state
		break;
		/*-----------------------------------------------------------*/
		case -2:
		// When current direction is wrong - attempt again
		ident_state = -3;	// goto -3 state
		break;
		/*-----------------------------------------------------------*/
		case -3:
			if(--us_tmp < 0)	//count here us1 - used as a timer
			{
				if(_IDENT_second_attempt() == 0)	// if this ident is first
				{
					us = 0;
					_set_IDENT_second_attempt(1);	// OK next ident will be second. Attempt to identify sigmaLss again
				}
				else	// this ident is second - wrong sigmaLss identified
				{
					us = ident_state;	// Return wrong sigma Lss identified
					_set_IDENT_second_attempt(0);		// Set IDENT attempts to 0
				
				}
				ident_state = 0;		// Return to INIT state
			}
		break;
		/*-----------------------------------------------------------*/
		default:
			Nop();
		break;

	}// end switch
	
	return(us);
}


/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*
 * Name:      
 * Purpose:   
 * 
*/
/******************************************************************************/

