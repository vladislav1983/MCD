/******************************************************************************/
/* 
 *                             TRACE VARIABLES
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef TRACE_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define TRACE_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "measure.h"
#include "vADC.h"
#include "SVM.h"
#include "Control_OpenLoop.h"

/*----------------------------------------------------------------------------*/
/* EXPORTED from other modules                                                */
/*----------------------------------------------------------------------------*/

extern const iolist iopar[];


/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/
static S16 dummy_null = 0;
static U16 TRSB_Clear_Flag = 0;
/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
S16 TRSB[TRACE_LEN][NB_TRACE_VARS] __attribute__ ((aligned));

volatile U16 trace_state;
volatile U16 trace_control;
volatile U16 STORE_POS;
U16 trace_counter;
S16 trace_trigger;
struct parlist_trace par_trace;

/* PARAMS STORED IN EEPROM */
volatile U16 trace_config;
volatile U16 trace_every_period;
U16 u16TraceParamNumber_1;
U16 u16TraceParamNumber_2;
U16 u16TraceParamNumber_3;
U16 u16TraceParamNumber_4;
U16 u16TraceParamNumber_5;
U16 u16TraceParamNumber_6;
U16 u16CompareParameter;
S16 s16CmpValue;


S16 *trace_source_addr[]  =
{
/* 0 */	(S16*)&dummy_null,
/*******************************************************/
/* MEASURE.H - Inverter Feedbacks*/
/* 1 */		(S16*)&smes.qIu,
/* 2 */		(S16*)&smes.qIv,
/* 3 */		(S16*)&smes.qIw,
/* 4 */		(S16*)&smes.qIdc,
/* 5 */		(S16*)&smes.qUdc,
/* 6 */		(S16*)&smes.qUuv,
/* 7 */		(S16*)&smes.qUvw,
/* 8 */		(S16*)&smes.qUwu,
/* 9 */		(S16*)&smes.qUnu,
/* 10 */	(S16*)&smes.qUnv,
/* 11 */	(S16*)&smes.qUnw,
/* 12 */	(S16*)&smes.qIalpha,
/* 13 */	(S16*)&smes.qIbeta,
/* 14 */	(S16*)&smes.qUalpha,
/* 15 */	(S16*)&smes.qUbeta,
/* 16 */	(S16*)&smes.qUd,
/* 17 */	(S16*)&smes.qUq,
/* 18 */	(S16*)&smes.qIs_amp,
/* 19 */	(S16*)&smes.qUs_amp,
/* 20 */	(S16*)&smes.qUs_amp,

/* ADC.H - Analog Channels in RAW format*/
/* 21 */	(S16*)&ADC[AN_0],
/* 22 */	(S16*)&ADC[AN_1],
/* 23 */	(S16*)&ADC[AN_2],
/* 24 */	(S16*)&ADC[AN_3],
/* 25 */	(S16*)&ADC[AN_4],
/* 26 */	(S16*)&ADC[AN_5],

/* MEASURE.H - ADC Channels in mV */
/* 27 */	(S16*)&adc_mV.AN_0_mV,
/* 28 */	(S16*)&adc_mV.AN_1_mV,
/* 29 */	(S16*)&adc_mV.AN_2_mV,
/* 30 */	(S16*)&adc_mV.AN_3_mV,
/* 31 */	(S16*)&adc_mV.AN_4_mV,
/* 32 */	(S16*)&adc_mV.AN_5_mV,

/* VF Control Paramters */
/* 33 */	(S16*)&vf_control.qVd_Gen_Ref,
/* 34 */	(S16*)&vf_control.qVq_Gen_Ref,
/* 35 */	(S16*)&vf_control.ref_angle,

/* PWM Module Duty cycle registers */
/* 36 */	(S16*)&PDC1,
/* 37 */	(S16*)&PDC2,
/* 38 */	(S16*)&PDC3,





/*******************************************************/
};

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

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
/******************************************************************************/
/*
* Purpose: Get nimber of parameters
* Input: none
* Output:  return number of params in system
*/
/******************************************************************************/
U16 IF_Get_TRSB_CntBytes(void)
{
	return(sizeof(TRSB));
}

/******************************************************************************/
/*
* Purpose: Trace variables in trace array. 
* Input: none
* Output:  none
*/
/******************************************************************************/
void do_trace(void)
{
	static U16 trace_period;
	S16 comp_param_local;
	
	
	par_trace.par_ptr[0] =  (S16*)trace_source_addr[u16TraceParamNumber_1];	
	par_trace.trace_display[0] = *par_trace.par_ptr[0];

	par_trace.par_ptr[1] =  (S16*)trace_source_addr[u16TraceParamNumber_2];
	par_trace.trace_display[1] = *par_trace.par_ptr[1];
	
	par_trace.par_ptr[2] =  (S16*)trace_source_addr[u16TraceParamNumber_3];
	par_trace.trace_display[2] = *par_trace.par_ptr[2];
	
	par_trace.par_ptr[3] =  (S16*)trace_source_addr[u16TraceParamNumber_4];
	par_trace.trace_display[3] = *par_trace.par_ptr[3];
	
	par_trace.par_ptr[4] =  (S16*)trace_source_addr[u16TraceParamNumber_5];
	par_trace.trace_display[4] = *par_trace.par_ptr[4];
	
	par_trace.par_ptr[5] =  (S16*)trace_source_addr[u16TraceParamNumber_6];
	par_trace.trace_display[5] = *par_trace.par_ptr[5];
	
	if(TRSB_Clear_Flag == 1)
	{
		
		if( Clear_Trace_Array() == S_OK)
		{
			_set_reset_trace_manual(0);
			_set_reset_trace(0);
			
			_set_trace_state(1);
			STORE_POS = 0;				//perform clear
			
			TRSB_Clear_Flag = 0;
		}
	
	}
	else if(_reset_trace_manual() || _reset_trace())
	{
		TRSB_Clear_Flag = 1;
	}
	else if(_condition_start_bit() && _trace_state() && !_trace_running())
	{
		
		comp_param_local = (S16)*trace_source_addr[u16CompareParameter];
		
		if(_more_than_bit())
		{
			if(comp_param_local > s16CmpValue)
			{
				_set_trace_running(1);
				//_set_reset_trace(1);
				STORE_POS = 0;
			}
		}
		else if(_less_than_bit())
		{
			if(comp_param_local < s16CmpValue)
			{
				_set_trace_running(1);
				//_set_reset_trace(1);
				STORE_POS = 0;
			}
		}	
		else if(_equal_bit())
		{
			if(comp_param_local == s16CmpValue)
			{
				_set_trace_running(1);
				//_set_reset_trace(1);
				STORE_POS = 0;
			}
		}		
		else if(_not_equal_bit())
		{
			if(comp_param_local != s16CmpValue)
			{
				_set_trace_running(1);
				//_set_reset_trace(1);
				STORE_POS = 0;
			}
		}	
	}
	if(_manual_start_trace() && !_trace_running())
	{
		_set_manual_start_trace(0);
		_set_trace_running(1);
		_set_trace_state(1);
	}
	
	
	if(_trace_running())	//if trace is running
	{

		if(STORE_POS >= TRACE_STEPS)
		{
			_set_trace_state(0);
			_set_trace_running(0);
		}
		else
		{
			if(!trace_period)
			{
				TRSB[STORE_POS][0] = (S16)*par_trace.par_ptr[0];
				TRSB[STORE_POS][1] = (S16)*par_trace.par_ptr[1];
				TRSB[STORE_POS][2] = (S16)*par_trace.par_ptr[2];
				TRSB[STORE_POS][3] = (S16)*par_trace.par_ptr[3];
				TRSB[STORE_POS][4] = (S16)*par_trace.par_ptr[4];
				TRSB[STORE_POS][5] = (S16)*par_trace.par_ptr[5];
				
				STORE_POS++;
				trace_period = trace_every_period;
			}
			else trace_period--;
		}	
	}	
}

/******************************************************************************/
/*
* Purpose: Trace array clear
* Input: none
* Output:  none
*/
/******************************************************************************/
HRESULT Clear_Trace_Array(void)
{
	static U16 steps;
	static U16 vars;
	
	TRSB[steps][vars] = 0;
	
	if(vars < NB_TRACE_VARS)
	{
		if(steps < TRACE_STEPS)
		{
			steps++;
		}
		else
		{
			vars++;
			steps = 0;
		}
	}
	else
	{
		steps = 0;
		vars = 0;
		return(S_OK);
	}

	return(S_FALSE);
	
}

/******************************************************************************/
/*
* Purpose: Trace Initialization.
* Input: none
* Output:  
*/
/******************************************************************************/
void trace_init_first(void)
{
	U16 steps = 0;
	U16 vars = 0;
	
	par_trace.trace_length = sizeof(trace_source_addr);
	
	
	for(vars = 0;vars <= NB_TRACE_VARS;vars++)
	{
		for(steps = 0;steps <= TRACE_STEPS;steps++)
		{
			TRSB[steps][vars] = 0;
		}
	}
	
	
	/* set pc control side config*/
	_set_reset_trace(1);
	
	/* set trace state*/
	_set_trace_state(0);			//put trace in blocked state

}



/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/

//-----------------------------------------------------------------------------



/******************************************************************************/
/*
 * Name:      
 * Purpose:   
 * 
*/
/******************************************************************************/
