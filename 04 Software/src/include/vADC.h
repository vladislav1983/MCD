#ifndef VADC_H
#define VADC_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* EXPORTED DATA                                                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define ADC_BITS				10
#define ADC_BUFFER_SIZE_BITS	16	
#define ADC_MEANING_BITS		15
#define ADC_SCALING_BITS		(ADC_MEANING_BITS - ADC_BITS - 1)					// 5 bits
#define ADC_SCALING				(1 << ADC_SCALING_BITS)								// 32
#define ADC_ANA_CH_SCALING		(1 << (ADC_MEANING_BITS) >> 1)						// 2 ^ 14 / 2 - ADC Analog Channel scaling 0V = -16384, 2.5V = 0, 5V = 16384
#define ADC_DOWNSCALE			(1 <<  (ADC_BUFFER_SIZE_BITS - ADC_BITS - 1)) 		//16 
/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
typedef enum eAdcChannel
{
    AN_0   = 0,
    AN_1   = 1,
    AN_2   = 2,
    AN_3   = 3,
    AN_4   = 4,
    AN_5   = 5,
    AN_Cnt = 6,

}AN;

extern U16 ADC[AN_Cnt];

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/
/* Motor Currents */
#define	ADC_Iu	( (ADC[AN_0] >> 1) & 0x7FFF )
#define	ADC_Iw	( (ADC[AN_1] >> 1) & 0x7FFF )

/* Phase Voltages */
#define	ADC_Uuv	( (ADC[AN_5] >> 1) & 0x7FFF )
#define	ADC_Uvw	( (ADC[AN_2] >> 1) & 0x7FFF )

/* DC Link Voltage and Current */
#define	ADC_Udc	( (ADC[AN_3] >> 1) & 0x7FFF )
#define	ADC_Idc	( (ADC[AN_4] >> 1) & 0x7FFF )

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void IF_ADC_INIT(void);
void ADC_Process(void);


#endif /* ADC_H */
