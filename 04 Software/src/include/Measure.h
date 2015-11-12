#ifndef MEASURE_H 
#define MEASURE_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define ADC_VREF_VOLT				5.0
#define ADC_VREF_BIN				32768.0
#define qADC_REF					(U32)(ADC_VREF_VOLT / 32768.0) 		// 5 * 65536


#define RMS_AVERAGE_POINTS			32 // 2^5
#define RMS_AVERAGE_POINTS_DIV		5 

#define DC_AVERAGE_COUNT			2048
#define DC_AVERAGE_DIVIDER			11

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

typedef struct sMeasure_Struct
{
	/* Inverter Angle */
	U16 InvAngle;
	
	/* Measured Values */
	S16 qIu;
	S16 qIw;
	S16 qIdc;
	
	S16 qUuv;
	S16 qUvw;
	S16 qUdc;
	
	/* Restored Three Phase Voltage Uwu and Current Iv */
	S16 qIv;
	S16 qUwu;
	
	/* Restored Three Phase Line Voltages */
	S16 qUnu;
	S16 qUnv;
	S16 qUnw;
	
	/* Ialpha and Ibeta after Forward Clarke Transform of Currents */
	S16 qIalpha;
	S16 qIbeta;
	
	/* Id and Iq after Forward Park Transform of currents */
	S16 qId;
	S16 qIq;
	
	/* Ualpha and Ubeta after Forward Clarke Transform of Three Phase Syatem Voltages */
	S16 qUalpha;
	S16 qUbeta;
	
	/* Ud and Uq after Forward Park Transform of line voltages */
	S16 qUd;
	S16 qUq;
	
	/* Resultant Current Vector Amplitude */
	S16 qIs_amp;
	
	/* Resultant Voltage Vector Amplitude */
	S16 qUs_amp;
	
}tsMeasure_Struct;

typedef struct sMeasure_Struct_T2
{
	S16 qUd_mean;
	S16 qUq_mean;
	
	S16 qUs_amp_mean;
	S16 qUs_rms;
	
	S16 Uinv_rms;
	S16 Iinv_rms;
	
	U16 Udc_V;
	U16 Idc_A;
	
}tsMeasure_Struct_T2;

typedef struct sADC_Offset
{	
	S16 Iu_off;
	S16 Iw_off;
	S16 Idc_off;
		
	S16 Uuv_off;
	S16 Uvw_off;
	S16 Udc_off;
	
}tsADC_Offset;

typedef struct sADC_Values_mV
{
	U16 AN_0_mV;
	U16 AN_1_mV;
	U16 AN_2_mV;
	U16 AN_3_mV;
	U16 AN_4_mV;
	U16 AN_5_mV;

}tsADC_Values_mV;

/*----------------------------------------------------------------------------*/
/* Exported data                                                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
extern tsMeasure_Struct 				smes;
#define _get_smes()			    		(&smes)

extern tsMeasure_Struct_T2				smes_t2;
#define _get_smes_t2()			    	(&smes_t2)

extern tsADC_Offset						adc_off;
#define _get_adc_off()			    	(&adc_off)

extern tsADC_Values_mV 					adc_mV;
#define _get_adc_mV()			    	(&adc_mV)

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
tsMeasure_Struct *measure( tsADC_Offset *of );
void adc_par_convert(void);
void measure_part_2( tsMeasure_Struct *ms, tsADC_Offset *of );
void MeasureTransform_T2(tsMeasure_Struct_T2 *ms2, tsMeasure_Struct *ms);
void measure_init(void);
HRESULT ANA_DC_Autotunning(void);
S16 Get_Vline_max(void);
S16 Get_Vphase_max_RMS(void);
S16 Get_Vphase_max(void);


#endif /* MEASURE_H */
