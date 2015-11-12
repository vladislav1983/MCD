/******************************************************************************/
/* 
 *                    OPEN LOOP VOLTAGE - FREQUENCY CONTROL 
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef CONTROL_OPENLOOP_C 
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define CONTROL_OPENLOOP_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "Control_OpenLoop.h"
#include "ACIM.h"
#include "SVM.h"
#include "measure.h"

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
#define MAX_FREQ	(500u)

/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Exported data to other modules                                             */
/*----------------------------------------------------------------------------*/
tsVF_In 		vf_control_in;
tsVF			vf_control;
/*----------------------------------------------------------------------------*/
/* Exported data from other modules                                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static U16 calc_delta_ref_angle(U16 Frequency_Hz);

/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*
* Purpose: Generate Three Phase Voltage - Open Loop
* Input: 
* Output: 
*/
/******************************************************************************/
void Voltage_Freq_Control(tsVF_In *vf_in, tsVF *vf, tsMeasure_Struct *ms, U16 drive_enable_state)
{
	S16 s16tmp = 0;
	
	/******************************************************************
	*
	* If drive is enabled
	* 
	*******************************************************************/
	if(drive_enable_state)
	{
		/******************************************************************
		*
		* Inverter Frequency Ramping.
		* Full Ramp Time = (65536/1600)*T1_PER_us*MAX_FREQ =  -> Time for full ramp from 0 to MAX_FREQ 
		* 
		*******************************************************************/
		(vf)->Freq_Ramped = CalcRamp1( (vf_in)->Frequency_Hz, MAX_FREQ, 0, 1200 ); 

		/******************************************************************
		*
		* Calc Voltage- Frequency constant - Calculate AMPLITUDE of voltage vector to be realized
		* 
		*******************************************************************/
		s16tmp = (_div_32( ((U32)(vf)->VF_constant * (vf)->Freq_Ramped * 10), (U16)motpar.UN_mot, _UDIV ));
		
		if(s16tmp <= (vf)->qVBoost) s16tmp = (vf)->qVBoost;
		if(s16tmp >= Get_Vline_max()) s16tmp = Get_Vline_max();
		
		/******************************************************************
		*
		* Inverter Voltage Ramping.
		* Full Ramp Time = (65536/1600)*T1_PER_us*UMAX =  -> Time for full ramp from 0 to UMAX 
		* 
		*******************************************************************/
		(vf)->qVd_Gen_Ref = CalcRamp2( s16tmp, Get_Vline_max(), 0, 20000 );
		(vf)->qVq_Gen_Ref = 0;
	
	}
	else
	{
		init_ramp1(0);
		init_ramp2(0);
		(vf)->qVd_Gen_Ref = 0;
		(vf)->qVq_Gen_Ref = 0;
	}
	
	/******************************************************************
	*
	* Calc Delta of Ref Angle. Inverter Angle += ref_angle_delta
	* 
	*******************************************************************/
	(vf)->ref_angle_delta = calc_delta_ref_angle( (vf)->Freq_Ramped );
	
	/******************************************************************
	*
	* RUN Space Vector Modulation
	* 
	*******************************************************************/
	SVM_GEN((vf)->qVd_Gen_Ref, (vf)->qVq_Gen_Ref, (vf)->ref_angle);
	
}

/******************************************************************************/
/*
* Purpose: Update Reference angle
* Input: none
* Output: Actual Reference Angle Of Inverter
*/
/******************************************************************************/
U16 VF_ref_angle_update(void)
{
	
	vf_control.ref_angle += vf_control.ref_angle_delta;
	
	return(vf_control.ref_angle);

}

/******************************************************************************/
/*
* Purpose: Init Voltage Frequency Parameters.
* Input: none
* Output: none
*/
/******************************************************************************/
void VF_ControlInit(void)
{
	init_ramp1(0);
	init_ramp2(0);
	
	vf_control.qVd_Gen_Ref = 0;
	vf_control.qVq_Gen_Ref = 0;
	
	/* Voltage Frequency Constant -  */
	vf_control.VF_constant = (U16)(((U32)(U16)motpar.UN_mot * SQRT2 * UMAX)  /  (U32)motpar.FN_mot );
	
	/* Boost Voltage. This is a minimal inverter voltage. Iverter voltage is limited to this voltage whem motor operate at low RPM's */
	//vf_control.qVBoost = ((U32)vf_control_in.VBoost_V * SQRT2 * 32767.0) / UMAX;
}

/******************************************************************************/
/*
* Purpose: Voltage Frequency Parameter Conversion. Run in background. Precalculate dynamic parameters entered from PC
* Input: none
* Output: none
*/
/******************************************************************************/
void VF_Par_Convert(void)
{
	
	/* Boost Voltage. This is a minimal inverter voltage. Iverter voltage is limited to this voltage whem motor operate at low RPM's */
	vf_control.qVBoost = ((U32)vf_control_in.VBoost_V * qSQRT2) / UMAX;

}

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*
 * Purpose: Calculate delta angle of inverter. In this case input parameter is 
 * 			flux speed!!! not a motor speed.
 * Input: Inverter Output Voltage Frequency,   
 * Output: none
 *
 * Some Clarifications:
 * omega = delta_angle/delta_t <-> 2*PI*f = delta_angle/delta_t
  where,
  delta_t - a shhort possible time -> PWM_PERIOD = TASK1_PERIOD.
  delta_angle - delta of angle of every interrupt
  -> delta angle = 2*PI*PWM_PERIOD*f
*/
/******************************************************************************/
static U16 calc_delta_ref_angle(U16 Frequency_Hz)
{
	U16 delta_ref_angle;
	
	delta_ref_angle = (U16)( 2 * qPI_deg * PWM_PERIOD * Frequency_Hz );

	return((U16)delta_ref_angle);
}
