#ifndef CONTROL_OPENLOOP_H 
#define CONTROL_OPENLOOP_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "measure.h"
/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/
typedef struct sVoltage_Freq_Control_In
{
	/* Params for Three Phase Voltage Inverter */
	U16 Voltage_Gen;	// Inverter Voltage V_RMS - Unused for now
	U16 Frequency_Hz;	// Inverter Frequency - Hz
	U16 VBoost_V;		// Inverter Boost Voltage - V_RMS

}tsVF_In;

/* Open Loop Control Output Parameters */
typedef struct sVoltage_Freq_Control
{
	/* Params for Three Phase Voltage Inverter */
	S16 qVd_Gen_Ref;		// Vd
	S16 qVq_Gen_Ref;		// Vq
	U16 Freq_Ramped;		// Ramped Frequency in Hz
	U16 ref_angle;			// Inverter Reference Angle
	U16 ref_angle_delta;
	
	U16 VF_constant;		// Voltage Frequency Constant - reffered to max line voltage vector
	
	U16 qVBoost;			// Boost Voltage
}tsVF;


/*----------------------------------------------------------------------------*/
/* Exported data                                                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
extern tsVF_In 							vf_control_in;
#define _get_vf_control_in()			(&vf_control_in)

extern tsVF								vf_control;
#define _get_vf_control()			    (&vf_control)

/******************************************************************************/
/*
* Purpose: Get Reference angle
* Input: 
* Output: 
*/
/******************************************************************************/
static inline U16 _GetRefAngle(void)
{
	return(vf_control.ref_angle);
}

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void Voltage_Freq_Control(tsVF_In *vf_in, tsVF *vf, tsMeasure_Struct *ms, U16 drive_enable_state);
U16 VF_ref_angle_update(void);
void VF_ControlInit(void);
void VF_Par_Convert(void);



#endif /* CONTROL_OPENLOOP_H */
