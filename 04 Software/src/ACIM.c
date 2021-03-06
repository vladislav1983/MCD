/******************************************************************************/
/* 
 *                              AC INDUCTION MOTOR PARAMETERS 
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef ACIM_C 
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define ACIM_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "ACIM.h"

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
tsMotPar			motpar; 		



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
* Purpose: 
* Input: 
* Output:  
*/
/******************************************************************************/

/******************************************************************************/
/*
* Purpose: 
* Input: 
* Output:  
* Notes:
* Alternatively, if the actual frequency, \omega,\, is written with units of radians per second
* (angular frequency), the normalized frequencies have units of radians per sample, 
* and the periodicity of the distribution is 2?.
*/
/******************************************************************************/
void ACIM_Init(void)
{
	tsMotPar *p;
	
	p = _get_motpar();
	
	// Normalized angular frequency omega_norm = 2*PI*f/fsample = 2*PI*f*Tsample
	(p)->omsN = (S32)(65536 * 4.096 * T1_PER_us/10000) * (p)->FN_mot / 4096;
	
	


}
/******************************************************************************/
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

