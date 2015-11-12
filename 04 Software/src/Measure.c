/******************************************************************************/
/* 
 *                               MEASUREMENT 
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef MEASURE_C 
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define MEASURE_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "Measure.h"
#include "statemachine.h"
#include "Control_OpenLoop.h"
#include "SVM.H"

/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/
typedef struct sTrip_Levels_Raw
{
	U16 Udc_trip_raw_max;
	U16 Udc_trip_raw_min;
	U16 Idc_trip_raw;
	U16 Inv_Voltage_trip_raw;
	U16 Inv_current_trip_raw;

}tsTrip_Levels_Raw;

typedef struct sGain_Corr
{		
	U16 Iu_gain;
	U16 Iw_gain;
	U16 Idc_gain;	
	
	U16 Uuv_gain;
	U16 Uvw_gain;
	U16 Udc_gain;
	
}tsGain_Corr;

/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/	
static S32 u32Ud_sum; 		// Voltages in dq coordinate
static S32 u32Uq_sum;

static U32 u32Us_amp_sum;	// Inverter Voltage Amplitude
static U32 u32Us_rms_fil; 	// Resultant filtered inverter rms voltage

static U32 u32Udc_sum;		// DC-Link Voltage and Current sum's
static U32 u32Idc_sum;

static tsTrip_Levels_Raw 	trip_levels;	// Values Trip Levels in Fractional raw format
static tsGain_Corr			gain_corr;

/* DC Autotunning routine local parameters */
static U16 					dc_average_counter;
static U32 					ANA0_DC_SUM;
static U32 					ANA1_DC_SUM;
static U32 					ANA2_DC_SUM;
static U32 					ANA3_DC_SUM;
static U32 					ANA4_DC_SUM;
static U32 					ANA5_DC_SUM;


/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Exported data to other modules                                             */
/*----------------------------------------------------------------------------*/
tsMeasure_Struct 			smes;
tsMeasure_Struct_T2			smes_t2;
tsADC_Offset				adc_off;
tsADC_Values_mV 			adc_mV;

/* Struvtures defined in SysParams.h */
struct sInv_Params_Struct 	sInv_Params;
struct sTrip_Levels_Struct 	sTrip_Levels;
struct sGain_Correction		sGain_Corr;
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
* Purpose: Measurement of values . Begin in PWM Interrupt
* Input: 
* Output:  
*/
/******************************************************************************/
tsMeasure_Struct *measure( tsADC_Offset *of )
{
	static U32 u32Id_sum; // Currents in dq coordinate
	static U32 u32Iq_sum;
	static U32 u32Is_amp_fil; // Realized Filtered Current
	tsMeasure_Struct *ms;
	U16 InvAngle;
	S16 s16tmp;
	
	ms = _get_smes();
	
	/******************************************************************
	*
	* Inverter Reference Angle Calculation
	*
	*******************************************************************/
	InvAngle = VF_ref_angle_update();
	(ms)->InvAngle = InvAngle;
	
	/******************************************************************
	* 0V   at ADC = 0   
	* 2.5V at ADC = 16383    
	* 5V   at ADC = 32768
	*******************************************************************/
	(ms)->qIu = (ADC_Iu - (of)->Iu_off) << 1;
	if( absi((ms)->qIu) > trip_levels.Inv_current_trip_raw) _set_inverter_overcurrent_fault(1);
	
	(ms)->qIw = (ADC_Iw - (of)->Iw_off) << 1;
	if( absi((ms)->qIw) > trip_levels.Inv_current_trip_raw) _set_inverter_overcurrent_fault(1);
	
	/* Phase V Current restore */
	(ms)->qIv = - ( (ms)->qIu + (ms)->qIw );
	if( absi((ms)->qIv) > trip_levels.Inv_current_trip_raw) _set_inverter_overcurrent_fault(1);	
	
	/******************************************************************
	* Forward Clarke Transformation of Phase Currents
	*
	*	Ialpha = Iu;
	*	Ibeta = (Iu + 2*Iv)/sqrt(3) = 
	*         = Iu * qOneBySq3 + 2 * qOneBySq3 * Iv
	*******************************************************************/
	(ms)->qIalpha = (ms)->qIu;
	
	(ms)->qIbeta = _fmul((ms)->qIu,/*x*/ qOneBySq3) + (2u * (S32)( _fmul( (ms)->qIv,/*x*/qOneBySq3 )) );
	
	/******************************************************************
	* Forward Park Transformation of line currents
	*
	*   qId = qIalpha*cos(InvAngle) + qIbeta*sin(InvAngle)
	*   qIq = qIbeta*cos(InvAngle)  - qIalpha*sin(InvAngle)
	*******************************************************************/
	(ms)->qId = _fmul( (ms)->qIalpha,/*x*/ cosine(InvAngle) ) + _fmul( (ms)->qIbeta, /*x*/ sine(InvAngle) );
	(ms)->qIq = _fmul( (ms)->qIbeta, /*x*/ cosine(InvAngle) ) - _fmul( (ms)->qIalpha,/*x*/ sine(InvAngle) );
	
	// Sum of dq currents
	u32Id_sum += (ms)->qId;
	u32Iq_sum += (ms)->qIq;
	
	// Resultant Vector - Is = SQRT( Id^2 + Iq^2 )
	s16tmp = sqrt_32( (S32)(ms)->qId*(ms)->qId + (S32)(ms)->qIq*(ms)->qIq );
	
	#define T_AMP_FILTER	70000ul	// filter time constant 70ms
	u32Is_amp_fil += (S32)( 65536.0*T1_PER_us/T_AMP_FILTER )*( s16tmp - Hi(u32Is_amp_fil) );
	(ms)->qIs_amp = Hi(u32Is_amp_fil);
	
	return(&smes);

}

/******************************************************************************/
/*
* Purpose: Measurement after PWM values calculation and SVM Generation.
* Input: none
* Output: none
*/
/******************************************************************************/
void measure_part_2( tsMeasure_Struct *ms, tsADC_Offset *of )
{
	U16 InvAngle;
	
	InvAngle = (ms)->InvAngle;
	
	/******************************************************************
	* 
	* Line Voltage Measure
	* 0 <= ADC_U <= 32768
	* Uuv_off - around 16384 -> 2.5V scaling at ADC
	*
	*******************************************************************/
	(ms)->qUuv = -((S16)ADC_Uuv - (of)->Uuv_off);
	
	(ms)->qUvw = -((S16)ADC_Uvw - (of)->Uvw_off);
	
	/* LINE Voltage Uwu Restore */
	(ms)->qUwu = (ms)->qUuv + (ms)->qUvw;
	
	/******************************************************************
	* Three Phase System PHASE Voltages Restore.
	*		Uu = (1/3)(Uuv - Uwu)
	*		Uv = (1/3)(Uvw - Uuv)
	*		Uw = (1/3)(Uwu - Uvw)
	*******************************************************************/
	//(ms)->qUnu = (_fmul(qOneBy3,/*x*/ ( (ms)->qUuv - (ms)->qUwu ))) << 1;
	//if( absi((ms)->qUnu) > trip_levels.Inv_Voltage_trip_raw) _set_inverter_overvoltage_fault(1);
	
	//(ms)->qUnv = (_fmul(qOneBy3,/*x*/ ( (ms)->qUvw - (ms)->qUuv ))) << 1;
	//if( absi((ms)->qUnv) > trip_levels.Inv_Voltage_trip_raw) _set_inverter_overvoltage_fault(1);
	
	//(ms)->qUnw = (_fmul(qOneBy3,/*x*/ ( (ms)->qUwu - (ms)->qUvw ))) << 1;
	//if( absi((ms)->qUnw) > trip_levels.Inv_Voltage_trip_raw) _set_inverter_overvoltage_fault(1);
	
	(ms)->qUnu = (S32)(65536/3)*((ms)->qUuv - (ms)->qUwu) >> 15; 
	(ms)->qUnv = (S32)(65536/3)*((ms)->qUvw - (ms)->qUuv) >> 15; 
	(ms)->qUnw = (S32)(65536/3)*((ms)->qUwu - (ms)->qUvw) >> 15; 
	
	
	/******************************************************************
	* Forward Clarke Transformation of Phase Voltages 
	*         Ualpha = Unu;
	*         Ubeta = (unu + 2*unw)/sqrt(3) =
	*               = Unu * qOneBySq3 + 2 * qOneBySq3 * Unw
	*******************************************************************/
	(ms)->qUalpha = (ms)->qUnu;
	
	//(ms)->qUbeta  = (S16)( (S32)_fmul( (ms)->qUnu,/*x*/qOneBySq3 ) + (2u * (_fmul( (ms)->qUnw,/*x*/qOneBySq3 ))) );
	(ms)->qUbeta = _fmul( (ms)->qUnu, qOneBySq3 ) + ( (S32)(65536/SQRT3) * ((ms)->qUnv * 2) >> 16);
	/******************************************************************
	* Forward Park Transformation
	*
	*   qUd = qUalpha*cos(InvAngle) + qUbeta*sin(InvAngle)
	*   qUq = qUbeta*cos(InvAngle)  - qUalpha*sin(InvAngle)
	*******************************************************************/
	(ms)->qUd = _fmul( (ms)->qUalpha,/*x*/ cosine(InvAngle) ) + _fmul( (ms)->qUbeta, /*x*/ sine(InvAngle) );
	(ms)->qUq = _fmul( (ms)->qUbeta, /*x*/ cosine(InvAngle) ) - _fmul( (ms)->qUalpha,/*x*/ sine(InvAngle) );
	
	// Sum of dq Inverter Voltages for mean value calculation in task 2
	u32Ud_sum += (ms)->qUd;
	u32Uq_sum += (ms)->qUq;
	
	// Resultant Vector Amplitude - Us = SQRT( Ud^2 + Uq^2 ) 
	(ms)->qUs_amp = sqrt_32( (S32)(ms)->qUd*(S32)(ms)->qUd + (S32)(ms)->qUq*(S32)(ms)->qUq );
	// Sumation for mean value calculation in task 2
	u32Us_amp_sum += (ms)->qUs_amp;
	
	/******************************************************************
	*
	*  DC Link Voltage and Current measure 
	* 
	*******************************************************************/
	(ms)->qUdc = ADC_Udc;// - (of)->Udc_off;  // This way of offset calibration is impossible, because of DC-Link voltage may be present
	u32Udc_sum  += (ms)->qUdc;
	
	if( absi((ms)->qUdc) > trip_levels.Udc_trip_raw_max) _set_dc_link_overvoltage_fault(1);
	else if( absi((ms)->qUdc) < trip_levels.Udc_trip_raw_min) _set_dc_link_undervoltage_fault(1);
	else _set_dc_link_undervoltage_fault(0);
	
	(ms)->qIdc = ADC_Idc - (of)->Idc_off;
	u32Idc_sum  += (ms)->qIdc;
	
	if( absi((ms)->qIdc) > trip_levels.Idc_trip_raw) _set_dc_link_overcurrent_fault(1);
}
/******************************************************************************/
/*
* Purpose: Measurement in TASK2
* Input: none
* Output: none
*
*  Convert Measured Values to Real Scale Values (Human Values)
*  This is done with Per Unit Syatem Scaling
*  If you dont know about it goto Wikipedia and read!
*  
*  Vpu  = Vact / Vbase
*  Vact = Vpu * Vbase
*  where, 
*  Vpu   - Per Unit Value. In our case 1.15 fractional value. Used for fractional mathematics
*  Vbase - This is base value. It is REAL Value.
*  Vact  - Actual REAL Value
*  After this bullshits we must multiply our fractional value with base value to produce actual value.
*/
/******************************************************************************/
void MeasureTransform_T2(tsMeasure_Struct_T2 *ms2, tsMeasure_Struct *ms)
{
	struct sInv_Params_Struct *ip;
	S16 s16tmp;
	S16 s16tmp2;
	S32 s32tmp;
	
	ip = &sInv_Params;
	
	/******************************************************************
	*
	*  DC Link Mean Voltage and Current calculation
	*  Mean_Value = Sum / (T2_PERIOD/T1_PERIOD)
	*  Real Value = Mean_Value * Base_Value
	* 
	*******************************************************************/
	s16tmp = _div_32( u32Udc_sum, (U16)(T2_PER_us/T1_PER_us), _UDIV);
	(ms2)->Udc_V = _fmul( s16tmp,  (ip)->DC_Link_Voltage_max);
	
	u32Udc_sum = 0;  // Clear sum buffer
	
	s16tmp= _div_32( u32Idc_sum, (U16)(T2_PER_us/T1_PER_us), _UDIV);
	(ms2)->Idc_A = _fmul( s16tmp,  (ip)->DC_Link_Current_max);
	
	u32Idc_sum = 0;  // Clear sum buffer
	
	/******************************************************************
	*
	*  Inverter Voltage Calculation
	*  Us_amplitude_mean_value = Us_amp_sum / (T2_PERIOD/T1_PERIOD)
	*  Urms = SQRT(3)/SQRT(2)*Us_amplitude_mean_value -> (SQRT(3) * Us_amp) / (SQRT(2))
	* 
	*******************************************************************/
	(ms2)->qUd_mean = _div_32( u32Ud_sum, (U16)(T2_PER_us/T1_PER_us), _SDIV);
	u32Ud_sum = 0;
	
	(ms2)->qUq_mean = _div_32( u32Uq_sum, (U16)(T2_PER_us/T1_PER_us), _SDIV);
	u32Uq_sum = 0;
	
	//s16tmp2 = _div_32( u32Us_amp_sum, (U16)(T2_PER_us/T1_PER_us), _UDIV);
	//u32Us_amp_sum = 0;
	//s16tmp  = (S32)(65536*SQRT3/SQRT2)*s16tmp2 >> 16;
	
	s16tmp = sqrt_32( (S32)(ms2)->qUd_mean*(S32)(ms2)->qUd_mean + (S32)(ms2)->qUq_mean*(S32)(ms2)->qUq_mean );
	s16tmp = (S32)(65536*SQRT3)*s16tmp >> 16;
	(ms2)->qUs_rms = s16tmp;
	
	
	#define T_UINV_AMP_FILTER	50000ul		// filter time constant 50ms
	 u32Us_rms_fil += (S32)( 65536.0*T2_PER_us/T_UINV_AMP_FILTER )*( s16tmp - Hi(u32Us_rms_fil) );
	(ms2)->Uinv_rms = _fmul( Hi(u32Us_rms_fil), (ip)->Inv_Voltage_max);
	
	/******************************************************************
	*
	*  Inverter Current Calculation
	*  Irms = (SQRT(3)/SQRT(2))*Is_amplitude
	* 
	*******************************************************************/
	s32tmp  = (U32)qSQRT3 * (S32)(ms)->qIs_amp; 	// tmp = SQRT(3)*qIs_amp
	s16tmp2 = _div_32( s32tmp, qSQRT2, _UDIV);		// tmp / SQRT(2)
	
	
	(ms2)->Iinv_rms = _fmul(s16tmp2, (ip)->Inv_Current_max);
	//s32tmp  = (S32)((U32)qSQRT3 * (ms)->qIs_amp) >> 15; 	// tmp = SQRT(3)*qIs_amp
	//(ms2)->Iinv_rms = (S32)((S32)s32tmp * (ip)->Inv_Current_max) >> 15;
	
}

/******************************************************************************/
/*
* Purpose: ADC Channels DC Autotunning
* Input: none
* Output: Autotunning State
*/
/******************************************************************************/
HRESULT ANA_DC_Autotunning(void)
{
	tsADC_Offset *of;
	HRESULT result;
	
	of = _get_adc_off();
	
	result = S_FALSE; 
	
	
	if(dc_average_counter)
	{
		// Sum offset of ADC Channels. 32 samples	
		ANA0_DC_SUM += ADC_Iu;
		ANA1_DC_SUM += ADC_Iw;
		ANA2_DC_SUM += ADC_Uvw;
		ANA3_DC_SUM += ADC_Udc;
		ANA4_DC_SUM += ADC_Idc;
		ANA5_DC_SUM += ADC_Uuv;
	
		dc_average_counter--;
	}
	else
	{
		// Divide by 32 to get acual offset value from sum buffers
		(of)->Iu_off  = ANA0_DC_SUM >> DC_AVERAGE_DIVIDER;
		(of)->Iw_off  = ANA1_DC_SUM >> DC_AVERAGE_DIVIDER;
		(of)->Uuv_off = ANA2_DC_SUM >> DC_AVERAGE_DIVIDER;
		(of)->Udc_off = ANA3_DC_SUM >> DC_AVERAGE_DIVIDER;
		(of)->Idc_off = ANA4_DC_SUM >> DC_AVERAGE_DIVIDER;
		(of)->Uvw_off = ANA5_DC_SUM >> DC_AVERAGE_DIVIDER;
		
		// Clear Sum Bufers
		ANA0_DC_SUM = 0;
		ANA1_DC_SUM = 0;
		ANA2_DC_SUM = 0;
		ANA3_DC_SUM = 0;
		ANA4_DC_SUM = 0;
		ANA5_DC_SUM = 0;
		
		dc_average_counter = DC_AVERAGE_COUNT;
		
		result = S_OK;
	}
	
	return(result);

}

/******************************************************************************/
/*
* Purpose: Convert ADC Values in real values. Run in background
* Input: none
* Output: none
*/
/******************************************************************************/
void adc_par_convert(void)
{
	volatile tsMeasure_Struct *p;
	U32 tmp;
	
	p = _get_smes();
	
	/* ADC in mV Calculation */
	tmp = ( (ADC[AN_0] >> 1) * ( ADC_VREF_VOLT * 1000) );
	adc_mV.AN_0_mV = tmp >> 15;
	
	tmp = ( (ADC[AN_1] >> 1) * ( ADC_VREF_VOLT * 1000) );
	adc_mV.AN_1_mV = tmp >> 15;
	
	tmp = ( (ADC[AN_2] >> 1) * ( ADC_VREF_VOLT * 1000) );
	adc_mV.AN_2_mV = tmp >> 15;
	
	tmp = ( (ADC[AN_3] >> 1) * ( ADC_VREF_VOLT * 1000) );
	adc_mV.AN_3_mV = tmp >> 15;
	
	tmp = ( (ADC[AN_4] >> 1) * ( ADC_VREF_VOLT * 1000) );
	adc_mV.AN_4_mV = tmp >> 15;
	
	tmp = ( (ADC[AN_5] >> 1) * ( ADC_VREF_VOLT * 1000) );
	adc_mV.AN_5_mV = tmp >> 15;
	
	/* Trip Levels Precalculation */	
	trip_levels.Udc_trip_raw_max     = (U16)( ((U32)sTrip_Levels.DC_Link_Trip_V_max     * 32767u) / sInv_Params.DC_Link_Voltage_max);
	trip_levels.Udc_trip_raw_min     = (U16)( ((U32)sTrip_Levels.DC_Link_Trip_V_min     * 32767u) / sInv_Params.DC_Link_Voltage_max);
	trip_levels.Idc_trip_raw         = (U16)( ((U32)sTrip_Levels.DC_Link_Current_Trip_A * 32767u) / sInv_Params.DC_Link_Current_max);
	trip_levels.Inv_Voltage_trip_raw = (U16)( ((U32)sTrip_Levels.Inv_Voltage_Trip_V     * 32767u) / sInv_Params.Inv_Voltage_max);
	trip_levels.Inv_current_trip_raw = (U16)( ((U32)sTrip_Levels.Inv_Current_Trip_A     * 32767u) / sInv_Params.Inv_Current_max);
	
	
}
/******************************************************************************/
/*
* Purpose: Init Measure module
* Input: none
* Output: none
*/
/******************************************************************************/
void measure_init(void)
{
	/******************************************************************
	*  Trip Levels Calculate 
	*  Convert trip levels Real Values (Human Values) to fractional trip levels
	*  This is done with Per Unit Syatem Scaling
	*  If you dont know about it GOTO Wikipedia and read for it!
	*  
	*  Vpu  = Vact / Vbase
	*  Vact = Vpu * Vbase
	*  where, 
	*  Vpu   - Per Unit Value. In our case 1.15 fractional value. Used for fractional mathematics
	*  Vbase - This is base value. It is REAL Value.
	*  Vact  - Actual REAL Value
	*  
	*  V_Trip(fract) = V_Trip(real) * 32767 / V_max(Base Value - Real)
	*  Note that due unsigned multiplication of real value we multiply by 32767 not 32768!!! - this is unsigned multiplication
	*  
	******************************************************************/
	
	trip_levels.Udc_trip_raw_max     = (U16)( ((U32)sTrip_Levels.DC_Link_Trip_V_max     * 32767u) / sInv_Params.DC_Link_Voltage_max);
	trip_levels.Udc_trip_raw_min     = (U16)( ((U32)sTrip_Levels.DC_Link_Trip_V_min     * 32767u) / sInv_Params.DC_Link_Voltage_max);
	trip_levels.Idc_trip_raw         = (U16)( ((U32)sTrip_Levels.DC_Link_Current_Trip_A * 32767u) / sInv_Params.DC_Link_Current_max);
	trip_levels.Inv_Voltage_trip_raw = (U16)( ((U32)sTrip_Levels.Inv_Voltage_Trip_V     * 32767u) / sInv_Params.Inv_Voltage_max);
	trip_levels.Inv_current_trip_raw = (U16)( ((U32)sTrip_Levels.Inv_Current_Trip_A     * 32767u) / sInv_Params.Inv_Current_max);
	
	/* Correction Coeffcients - not used for now*/
	gain_corr.Iu_gain  = (U16)( (S32)sInv_Params.Inv_Current_max     * sGain_Corr.Iu_corr  / 1000u );
	gain_corr.Iw_gain  = (U16)( (S32)sInv_Params.Inv_Current_max     * sGain_Corr.Iw_corr  / 1000u );
	gain_corr.Uuv_gain = (U16)( (S32)sInv_Params.Inv_Voltage_max     * sGain_Corr.Uuv_corr / 1000u );
	gain_corr.Uvw_gain = (U16)( (S32)sInv_Params.Inv_Voltage_max     * sGain_Corr.Uvw_corr / 1000u );
	gain_corr.Udc_gain = (U16)( (S32)sInv_Params.DC_Link_Voltage_max * sGain_Corr.Udc_corr / 1000u );
	gain_corr.Idc_gain = (U16)( (S32)sInv_Params.DC_Link_Current_max * sGain_Corr.Idc_corr / 1000u );
	
	ANA0_DC_SUM = 0;
	ANA1_DC_SUM = 0;
	ANA2_DC_SUM = 0;
	ANA3_DC_SUM = 0;
	ANA4_DC_SUM = 0;
	ANA5_DC_SUM = 0;
			
	dc_average_counter = DC_AVERAGE_COUNT;
}

/******************************************************************************/
/*
* Purpose: Get Max Realized LINE Voltage Vector
* Input: none
* Output: Normalized Voltage Vector. Amplitude voltage vector = Udc. Udc - actual DC-Link voltage
*/
/******************************************************************************/
S16 Get_Vline_max(void)
{
	S16 tmp;
	
	tmp = (U16)(smes.qUdc >> 3);	// scale to UMAX (4096) 1.12 value.
	
	return(tmp);

}
/******************************************************************************/
/*
* Purpose: Get Max Realized RMS PHASE Voltage Vector
* Input: none
* Output: Normalized Voltage Vector. Amplitude of max RMS voltage vector 1/SQRT3*Udc. Udc - actual DC-Link voltage
*/
/******************************************************************************/
S16 Get_Vphase_max_RMS(void)
{
	S16 tmp;
	
	tmp = (U16)((U32)(65536/SQRT3)*smes.qUdc >> 16);	// scale to 1.15
	tmp >>= 3;	// scale to UMAX (4096) 1.12 value.
	
	return(tmp);

}

/******************************************************************************/
/*
* Purpose: Get Max Realized PHASE Voltage Vector
* Input: none
* Output: Voltage Vector. Amplitude of max voltage vector 2/3*Udc. Udc - actual DC-Link voltage
*/
/******************************************************************************/
S16 Get_Vphase_max(void)
{
	S16 tmp;
	
	tmp = (U16)((U32)(2*65536/3)*smes.qUdc >> 16);	// scale to 1.15
	tmp >>= 3;	// scale to UMAX (4096) 1.12 value.
	
	return(tmp);

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

