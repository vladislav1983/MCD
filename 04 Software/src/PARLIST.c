/******************************************************************************/
/* 
 *       			 S Y S T E M      P A R A M E T E R S                 
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef PARLIST_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define PARLIST_C
#endif

/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "measure.h"
#include "statemachine.h"
#include "Control_OpenLoop.h"
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
U16 release;    		
const U16 version = FIRMWARE_BASE_VERSION + FIRMWARE_SUB_VERSION;  
const U16 C30_Version = __C30_VERSION__; 

/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/* STATEMACHINE.H */
extern AppST_States appstate;

/* TRACE.C */
extern U16 trace_config;
extern volatile U16 trace_control;
extern U16 STORE_POS;
extern U16 u16TraceParamNumber_1;
extern U16 u16TraceParamNumber_2;
extern U16 u16TraceParamNumber_3;
extern U16 u16TraceParamNumber_4;
extern U16 u16TraceParamNumber_5;
extern U16 u16TraceParamNumber_6;
extern S16 s16CmpValue;
extern U16 trace_state;
extern U16 u16CompareParameter;
extern struct parlist_trace par_trace;
extern volatile U16 trace_every_period;

/* EEPROM.C */
extern U16 EE_CHKS;
extern U16 EE_Valid;
extern U16 EE_par_update;
/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/
const iolist iopar[] =
{  /* SYSPARAMS.H */
   // Inverter Parameters
  {1,"DC Link Voltage max",			(U16 *)&sInv_Params.DC_Link_Voltage_max,	(U16 *)&sInv_Params.DC_Link_Voltage_max, 	2,"unr", 0xFFFFul, 0,0,0,10},	
  {2,"DC Link Current max",			(U16 *)&sInv_Params.DC_Link_Current_max,	(U16 *)&sInv_Params.DC_Link_Current_max, 	2,"unr", 0xFFFFul, 0,0,0,10},	
  {3,"Inverter Voltage max",		(U16 *)&sInv_Params.Inv_Voltage_max,		(U16 *)&sInv_Params.Inv_Voltage_max, 		2,"unr", 0xFFFFul, 0,0,0,10},
  {4,"Inverter Current max",		(U16 *)&sInv_Params.Inv_Current_max,		(U16 *)&sInv_Params.Inv_Current_max, 		2,"unr", 0xFFFFul, 0,0,0,10},
  // Inverter Trip Levels
  {5,"DC Link MAX Trip Voltage",	(U16 *)&sTrip_Levels.DC_Link_Trip_V_max,	(U16 *)&sTrip_Levels.DC_Link_Trip_V_max, 	2,"unr", 0xFFFFul, 0,0,0,10},
  {6,"DC Link MIN Trip Voltage",	(U16 *)&sTrip_Levels.DC_Link_Trip_V_min,	(U16 *)&sTrip_Levels.DC_Link_Trip_V_min, 	2,"unr", 0xFFFFul, 0,0,0,10},
  {7,"DC Link Trip Current",		(U16 *)&sTrip_Levels.DC_Link_Current_Trip_A,(U16 *)&sTrip_Levels.DC_Link_Current_Trip_A,2,"unr", 0xFFFFul, 0,0,0,10},
  {8,"Inverter Trip Voltage",		(U16 *)&sTrip_Levels.Inv_Voltage_Trip_V,	(U16 *)&sTrip_Levels.Inv_Voltage_Trip_V,	2,"unr", 0xFFFFul, 0,0,0,10},
  {9,"Inverter Trip Current",		(U16 *)&sTrip_Levels.Inv_Current_Trip_A,	(U16 *)&sTrip_Levels.Inv_Current_Trip_A,	2,"unr", 0xFFFFul, 0,0,0,10},
  {10,"Normalization Current",		(S16 *)&sInv_Params.I_norm,					(S16 *)&sInv_Params.I_norm,					2,"snr", 0xFFFFul, 0,0,0,10},
  
  /* ACIM.H */
  // Motor Parameters
  {20,"Motor Poles ",				 (S16 *)&motpar.Poles_mot,					(S16 *)&motpar.Poles_mot,					2,"snr", 0xFFFFul, 0,0,0,10},
  {21,"Motor Nominal Speed RPM",	 (S16 *)&motpar.RPMN_mot,					(S16 *)&motpar.RPMN_mot,					2,"snr", 0xFFFFul, 0,0,0,10},
  {22,"Motor Nominal Supply Freq",	 (S16 *)&motpar.FN_mot,						(S16 *)&motpar.FN_mot,						2,"snr", 0xFFFFul, 0,0,0,10},
  {23,"Motor Nominal Supply Voltage",(S16 *)&motpar.UN_mot,						(S16 *)&motpar.UN_mot,						2,"snr", 0xFFFFul, 0,0,0,10},
  {24,"Motor Rated Current",		 (S16 *)&motpar.IN_mot,						(S16 *)&motpar.IN_mot,						2,"snr", 0xFFFFul, 0,0,0,10},
  {25,"Motor No-Load Current",		 (S16 *)&motpar.I0,							(S16 *)&motpar.I0,							2,"snr", 0xFFFFul, 0,0,0,10},
  {26,"Sigma Lss",		 			 (S16 *)&motpar.SigmaLss,					(S16 *)&motpar.SigmaLss,					2,"snr", 0xFFFFul, 0,0,0,10},
  {27,"Rs",		 			 		 (S16 *)&motpar.Rs,							(S16 *)&motpar.Rs,							2,"snr", 0xFFFFul, 0,0,0,10},
  {28,"Rr",		 			 		 (S16 *)&motpar.Rr,							(S16 *)&motpar.Rr,							2,"snr", 0xFFFFul, 0,0,0,10},
  {29,"Ls",		 			 		 (S16 *)&motpar.Ls,							(S16 *)&motpar.Ls,							2,"snr", 0xFFFFul, 0,0,0,10},
  {30,"cos phi",		 			 (S16 *)&motpar.cosphi,						(S16 *)&motpar.cosphi,						2,"snr", 0xFFFFul, 0,0,0,100},
  {31,"Sigma Lss Identified",		 (S16 *)&motpar.SigmaLss_ID,				(S16 *)&motpar.SigmaLss_ID,					2,"srr", 0xFFFFul, 0,5,0,100},
  {32,"Electrical Torque",		 	 (S16 *)&motpar.T_el,						(S16 *)&motpar.T_el,						2,"srr", 0xFFFFul, 0,5,0,10},
  {33,"Motor Rated Torque",		 	 (S16 *)&motpar.TN_mot,						(S16 *)&motpar.TN_mot,						2,"snr", 0xFFFFul, 0,0,0,10},
  
  /* Feedbacks */
  // ADC Voltage
  {50,"AN_0 (Iu)",					(U16 *)&adc_mV.AN_0_mV,				(U16 *)&adc_mV.AN_0_mV,	 				2,"urr", 0xFFFFul, 			0,5,0,1},
  {51,"AN_1 (Iw)",					(U16 *)&adc_mV.AN_1_mV,				(U16 *)&adc_mV.AN_1_mV,	 				2,"urr", 0xFFFFul, 			0,5,0,1},
  {52,"AN_2 (Uuv)",					(U16 *)&adc_mV.AN_2_mV,				(U16 *)&adc_mV.AN_2_mV,	 				2,"urr", 0xFFFFul, 			0,5,0,1},
  {53,"AN_3 (Udc)",					(U16 *)&adc_mV.AN_3_mV,				(U16 *)&adc_mV.AN_3_mV,	 				2,"urr", 0xFFFFul, 			0,5,0,1},
  {54,"AN_4 (Idc)",					(U16 *)&adc_mV.AN_4_mV,				(U16 *)&adc_mV.AN_4_mV,	 				2,"urr", 0xFFFFul, 			0,5,0,1},
  {55,"AN_5 (Uvw)",					(U16 *)&adc_mV.AN_5_mV,				(U16 *)&adc_mV.AN_5_mV,	 				2,"urr", 0xFFFFul, 			0,5,0,1},
  {56,"Inverter Voltage (RMS)",		(U16 *)&smes_t2.Uinv_rms,			(U16 *)&smes_t2.Uinv_rms,	 			2,"urr", 0xFFFFul, 			0,5,0,10},
  {57,"Inverter Current (RMS)",		(U16 *)&smes_t2.Iinv_rms,			(U16 *)&smes_t2.Iinv_rms,	 			2,"urr", 0xFFFFul, 			0,5,0,10},
  {58,"DC Link Voltage",			(U16 *)&smes_t2.Udc_V,				(U16 *)&smes_t2.Udc_V,	 				2,"urr", 0xFFFFul, 			0,5,0,10},
  {59,"DC Link Current",			(U16 *)&smes_t2.Idc_A,				(U16 *)&smes_t2.Idc_A,	 				2,"urr", 0xFFFFul, 			0,5,0,10},
  {60,"Inverter Unfil Voltage (RMS)",	(U16 *)&smes_t2.qUs_rms,			(U16 *)&smes_t2.qUs_rms,	 			2,"urr", 0xFFFFul, 			0,5,0,10},
  
  
	/* TRACE.C */
  {300,"TRACE CONFIG",				(U16 *)&trace_config,				(U16 *)&trace_config,					2,"bnr",	0xFFFFul,		 0,0,0,1},
  {301,"TRACE CONTROL",				(U16 *)&trace_control,				(U16 *)&trace_control,	        		2,"brr",	0xFFFFul,		 0,0,0,1},
  {302,"Trace every period",		(U16 *)&trace_every_period,			(U16 *)&trace_every_period,	  			2,"unr",	0xFFFFul,		 0,0,0,1},
  {303,"Compare Value",				(S16 *)&s16CmpValue,				(S16 *)&s16CmpValue,	        		2,"snr",	32767ul,	-32768,0,0,1},
  {304,"Compare Parameter Number",	(U16 *)&u16CompareParameter,		(U16 *)&u16CompareParameter,	  		2,"unr",	0xFFFFul,		 0,0,0,1},
  {305,"TRACE STATE",				(U16 *)&trace_state,				(U16 *)&trace_state,	        		2,"brr",	0xFFFFul,		 0,5,0,1},
  {309,"STORE_POS",					(U16 *)&STORE_POS,					(U16 *)&STORE_POS,						2,"urr",	0xFFFFul,		 0,5,0,1},   
  {310,"Trace Parameter Nr:1",		(U16 *)&u16TraceParamNumber_1,		(U16 *)&u16TraceParamNumber_1,			2,"unr",	0xFFFFul,		 0,0,0,1},   
  {311,"Trace Parameter Nr:2",		(U16 *)&u16TraceParamNumber_2,		(U16 *)&u16TraceParamNumber_2,			2,"unr",	0xFFFFul,	 	 0,0,0,1},
  {312,"Trace Parameter Nr:3",		(U16 *)&u16TraceParamNumber_3,		(U16 *)&u16TraceParamNumber_3,			2,"unr",	0xFFFFul,	 	 0,0,0,1},
  {313,"Trace Parameter Nr:4",		(U16 *)&u16TraceParamNumber_4,		(U16 *)&u16TraceParamNumber_4,			2,"unr",	0xFFFFul,	 	 0,0,0,1},
  {314,"Trace Parameter Nr:5",		(U16 *)&u16TraceParamNumber_5,		(U16 *)&u16TraceParamNumber_5,			2,"unr",	0xFFFFul,	 	 0,0,0,1},
  {315,"Trace Parameter 1 Value =",	(S16 *)&par_trace.trace_display[0],	(S16 *)&par_trace.trace_display[0],		2,"srr",	32767ul,	-32768,5,0,1}, 
  {316,"Trace Parameter 2 Value =",	(S16 *)&par_trace.trace_display[1],	(S16 *)&par_trace.trace_display[1],		2,"srr",	32767ul,	-32768,5,0,1},
  {317,"Trace Parameter 3 Value =",	(S16 *)&par_trace.trace_display[2],	(S16 *)&par_trace.trace_display[2],		2,"srr",	32767ul,	-32768,5,0,1},  
  {318,"Trace Parameter 4 Value =",	(S16 *)&par_trace.trace_display[3],	(S16 *)&par_trace.trace_display[3],		2,"srr",	32767ul,	-32768,5,0,1},  
  {319,"Trace Parameter 5 Value =",	(S16 *)&par_trace.trace_display[4],	(S16 *)&par_trace.trace_display[4],		2,"srr",	32767ul,	-32768,5,0,1},  
  {320,"Trace array length",		(U16 *)&par_trace.trace_length,		(U16 *)&par_trace.trace_length,	 		2,"urr",	0xFFFFul,		 0,5,0,1},
  {321,"Trace Parameter Nr:6",		(U16 *)&u16TraceParamNumber_6,		(U16 *)&u16TraceParamNumber_6,			2,"unr",	0xFFFFul,	 	 0,0,0,1},
  {322,"Trace Parameter 6 Value =",	(S16 *)&par_trace.trace_display[5],	(S16 *)&par_trace.trace_display[5],		2,"srr",	32767ul,	-32768,5,0,1},
  
  /* Voltage Frequency Control - Openloop */
  {400,"Voltage",					(U16 *)&vf_control_in.Voltage_Gen,	(U16 *)&vf_control_in.Voltage_Gen,		2,"urr",	0xFFFFul,		 0,0,0,1},
  {401,"Frequency",					(U16 *)&vf_control_in.Frequency_Hz,	(U16 *)&vf_control_in.Frequency_Hz,		2,"urr",	0xFFFFul,		 0,0,0,1},
  {402,"Vd Ref",					(S16 *)&vf_control.qVd_Gen_Ref,		(S16 *)&vf_control.qVd_Gen_Ref,			2,"srr",	32767ul,	-32768,0,0,1},
  {403,"Vq Ref",					(S16 *)&vf_control.qVq_Gen_Ref,		(S16 *)&vf_control.qVq_Gen_Ref,			2,"srr",	32767ul,	-32768,0,0,1},
  {404,"Ref Angle Delta",			(U16 *)&vf_control.ref_angle_delta,	(U16 *)&vf_control.ref_angle_delta,		2,"urr",	0xFFFFul,	    0,5,0,1},
  {405,"U Boost",					(U16 *)&vf_control_in.VBoost_V,		(U16 *)&vf_control_in.VBoost_V,			2,"unr",	0xFFFFul,	    0,0,0,1},
  
  
  
  
  
  
  
  
  /* ---------------------------------------------------------------------------------------------------------------------------------------------- */
  /* Control Registers                                                                                                                              */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------- */
  
  
  /* TASK */  
  {900,"Task1 Period",				(U16 *)&sTimes.Task1Time_us,		(U16 *)&sTimes.Task1Time_us,			4,"urr",	-1, 			0,5,0,1000},
  {901,"Task1 Stack Usage",			(U16 *)&sTimes.Task1_Stack_Usage,	(U16 *)&sTimes.Task1_Stack_Usage,		2,"urr",	0xFFFFul,	    0,5,0,1},
  {902,"Free Stack",				(U16 *)&sTimes.Stack_Free,			(U16 *)&sTimes.Stack_Free,				2,"urr",	0xFFFFul,	    0,5,0,1},
  {903,"Stack Limit Pointer",		(U16 *)&SPLIM,						(U16 *)&SPLIM,							2,"urr",	0xFFFFul,	  	0,5,0,1},
  {904,"Stack Size",				(U16 *)&sTimes.Stack_Size,			(U16 *)&sTimes.Stack_Size,				2,"urr",	0xFFFFul,	  	0,5,0,1},
  {905,"Task2 Period",				(U16 *)&sTimes.Task2Time_us,		(U16 *)&sTimes.Task2Time_us,			4,"urr",	-1, 			0,5,0,1000},
  {906,"Task2 Stack Usage",			(U16 *)&sTimes.Task2_Stack_Usage,	(U16 *)&sTimes.Task2_Stack_Usage,		2,"urr",	0xFFFFul,	    0,5,0,1},
  {907,"Task1 Run Time",			(U16 *)&sTimes.Task1RunTime_us,		(U16 *)&sTimes.Task1RunTime_us,			4,"urr",	-1, 			0,5,0,1000},
  {908,"Task2 Run Time",			(U16 *)&sTimes.Task2RunTime_us,		(U16 *)&sTimes.Task2RunTime_us,			4,"urr",	-1, 			0,5,0,1000},
  
  
  
  /* CPU IO Registers */
  {910,"PORTF",						(U16 *)&PORTF,						(U16 *)&PORTF,							2,"brr",	0xFFFFul,	  	0,5,0,1},
  {911,"PORTD",						(U16 *)&PORTD,						(U16 *)&PORTD,							2,"brr",	0xFFFFul,	  	0,5,0,1},
  {912,"PORTG",						(U16 *)&PORTG,						(U16 *)&PORTG,							2,"brr",	0xFFFFul,	  	0,5,0,1},
  
  /* statemachine.h */
  {913,"SYS_CONTROL",				(U16 *)&SYS_CONTROL,				(U16 *)&SYS_CONTROL,					2,"brr",	0xFFFFul,	  	0,5,0,1},
  {914,"PC_CONTROL",				(U16 *)&PC_CONTROL,					(U16 *)&PC_CONTROL,						2,"brr",	0xFFFFul,	  	0,0,0,1},
  {915,"ERR_CONTROL",				(U16 *)&ERR_CONTROL,				(U16 *)&ERR_CONTROL,					2,"brr",	0xFFFFul,	  	0,5,0,1},
  {917,"App State",					(U16 *)&appstate,					(U16 *)&appstate,						2,"urr",	0xFFFFul,	  	0,5,0,1},
  
  /* DigitalIo.h - Digital IO Test mode */
  {916,"DIO_TEST_REG",				(U16 *)&DIO_TEST_REG,				(U16 *)&DIO_TEST_REG,					2,"brr",	0xFFFFul,	  	0,0,0,1}, 
  
  /* SYSTEM PARAMETERS */
  /* EEPROM.C */	
  {950,"EEPROM Checksum",			(U16 *)&EE_CHKS,					(U16 *)&EE_CHKS,						2,"urr",	0xFFFFul,	  	0,5,0,1},
  {951,"EEPROM Valid (1000)",		(U16 *)&EE_Valid,					(U16 *)&EE_Valid,						2,"unr",	0xFFFFul,	  	0,0,0,1},
  {952,"EEPROM Update (Debug)",		(U16 *)&EE_par_update,				(U16 *)&EE_par_update,					2,"urr",	0xFFFFul,	  	0,5,0,1},
  {953,"EE_ERR_REG",				(U16 *)&EE_ERR_REG,					(U16 *)&EE_ERR_REG,						2,"brr",	0xFFFFul,	  	0,5,0,1},
  
  
   /* Soft Version */  
  {995,"Compiled with C30 VERSION", (U16 *)&C30_Version,			    (U16 *)&C30_Version,					2,"uoo",	65535u ,	  0,5,0, 100},
  {996,__TIME__,           			(U16 *)&release,					(U16 *)&release,						2,"uoo",	65535u ,	  0,5,0, 1},
  {997,__DATE__,           			(U16 *)&release,					(U16 *)&release,						2,"uoo",	65535u ,	  0,5,0, 1}, 
  {998,"Release",           		(U16 *)&release,					(U16 *)&release,						2,"unr",	65535u ,	  0,0,0, 1},
  {999,"Version",           		(U16 *)&version,					(U16 *)&version,						2,"uoo",	65535u ,      0,5,0, 100},
  {  0,"",                  		NULL,								NULL,									0,"srr",	0u ,		  0,0,1, 1}
};
/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/



/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
* Purpose: Get nimber of parameters
* Output:  return number of params in system
*/
/******************************************************************************/
U16 IF_Parlist_GetCntMembers(void)
{
    return( sizeof(iopar)/sizeof(iopar[0]) );
}

/******************************************************************************/
/*
* Purpose: Get bytes of "iopar" structure
* Output:  return number bytes
*/
/******************************************************************************/
U16 IF_Parlist_GetCntBytes(void)
{
    return( sizeof(iopar) );
}

/******************************************************************************/
/*
* Purpose: Get desired param number
* Output:  return pointer to desired parameter
*/
/******************************************************************************/
iolist* IF_Parlist_bnu(U16 U32PrmNumber)
{
    iolist* pio;

    // get pointer to current parameter
    for( pio=(iolist*)iopar; (0 != pio->number) && (U32PrmNumber != pio->number); pio++ )
        ;
    
    return(pio);
}
/******************************************************************************/
/*
 * Purpose: Checking for parameters limit. Check critical parameters for correct values. Run in background loop in main.c
 * Inputs:  none
 * Output:	none
 */
/******************************************************************************/
void Params_check_limit(void)
{

}
/******************************************************************************/
/*
 * Purpose: Parameters conversion. Called every TASK 2.
 * Inputs:  none
 * Output:	none
 */
/******************************************************************************/
void Params_Conversion(void)
{








}




