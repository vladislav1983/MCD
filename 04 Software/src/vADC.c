/******************************************************************************/
/* 
 *                          ADC IMPLEMENTATION     
 *       
*/
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef VADC_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define VADC_C
#endif

/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/
#define ADC_MUXA_BITS_MASK				0b1111111100000000
#define ADC_MUXA_AN0_AN1_AN2_AN5		0b0000000000000101
#define ADC_MUXA_AN3_AN4				0b0000000000100100
/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/
/* MUXA First Configuration */
/* AN5 -> CH0 (Uuv) */
/* AN0 -> CH1 (Iu)  */
/* AN1 -> CH2 (Iw)  */
/* AN2 -> CH3 (Uvw) */
#define LOAD_MUXA_CFG_FIRST()			ADCHS &= ADC_MUXA_BITS_MASK; 	\
										ADCHS |= ADC_MUXA_AN0_AN1_AN2_AN5	
	
/* MUXA Second Configuration. CH0, CH1 is used. */
/* AN4 -> CH0 (Idc) 		*/
/* AN3 -> CH1 (Udc) 		*/
/* AN2 -> CH2 - Do Not Convert This Channel	*/
/* AN5 -> CH3 - Do Not Convert This Channel */
#define LOAD_MUXA_CFG_SECOND()			ADCHS &= ADC_MUXA_BITS_MASK; 	\
										ADCHS |= ADC_MUXA_AN3_AN4				
																					
																			
#define CONVERT_CH0_CH1_CH2_CH3()		ADCON2bits.CHPS 	= 0b10
#define CONVERT_CH0_CH1()				ADCON2bits.CHPS 	= 0b01
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
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
U16 ADC[AN_Cnt];
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
 * Name:      ADC_INIT
 * Purpose:   INIT ADC
 * 
*/
/******************************************************************************/
void IF_ADC_INIT(void)
{
	//	ADCCON1
	// |--------------------------------------------------------------------------------------------
    // | ADON	x	ADSIDL	x	x	x	FORM<1:0>	SSRC<2:0>	x	   SIMSAM	 ASAM	SAMP   DONE
    // |--------------------------------------------------------------------------------------------
    // |  0		x	   0	x	x	x	   10			111		x	   	1	  	  0	 	 x		 x
    // |--------------------------------------------------------------------------------------------
	
	ADCON1 = 0x0000;
	
	ADCON1bits.ADSIDL 	= 0; 			// Continue module operation in Idle mode
	ADCON1bits.FORM 	= (0b10); 		// 10 = Fractional (DOUT = dddd dddd dd00 0000)
	ADCON1bits.SSRC 	= (0b111); 		// GP Timer3 compare ends sampling and starts conversion
	ADCON1bits.SIMSAM 	= 1;			// Samples CH0, CH1, CH2, CH3 simultaneously (when CHPS = 1x)
	ADCON1bits.ASAM 	= 0; 			// 0 = Sampling begins when SAMP bit set
	
	//	ADCCON2
	// |--------------------------------------------------------------------------------------------
    // | VCFG<2:0>	!	x	CSCNA	CHPS<1:0>     BUFS	    x	  SMPI<3:0>	 BUFM	ALTS	
    // |--------------------------------------------------------------------------------------------
    // |  	000	    0	x	  0		   10	        x	   	x	    0000	   0	  0
    // |--------------------------------------------------------------------------------------------	
    
    ADCON2 = 0x0000;
    
    ADCON2bits.VCFG 	= (0b000); 		// AVDD AVSS
    ADCON2bits.CSCNA 	= 0; 			// Do not scan inputs
    ADCON2bits.CHPS 	= 0b10;			// Converts CH0, CH1, CH2 and CH3
    ADCON2bits.SMPI 	= (0b0000); 	// Interrupts at the completion of conversion for each sample/convert sequence
    ADCON2bits.BUFM 	= 0; 			// Buffer configured as one 16-word buffer ADCBUF(15...0.)
    ADCON2bits.ALTS 	= 0; 			// 0 = Always use MUX A input multiplexer settings
    
	//	ADCCON3
	// |--------------------------------------------------------------------------------------------
    // | x	x	x	SAMC<4:0>	ADRC	x	ADCS<5:0>
    // |--------------------------------------------------------------------------------------------
    // | x	x	x	  3 		 0		x	   6
    // |--------------------------------------------------------------------------------------------
    //	Minimum Tad = 95.24 ns  	-> Minimum ADC conversion period (See errata clarification / dsPIC30F6015 /. ADC module max sample rate = 750ksps)
    //	Tcy = 31.25 ns 				-> CPU inctruction cycle period
    //	ADCS<5:0> = (2 * (Tad / Tcy) - 1) = 5.095 -> We select ADCS<5:0> = 6
    //	
    //	Actual Tad = (Tcy / 2) * (ADCS<5:0> +1) = 109.375 ns
    //	If SSRC<2:0> = 111 and SAMC<4:0> = 00001
    //	Since,
    //  Conversion Time = 12Tad = 1312.5 ns
	//	Sampling Time   = Acquisition Time + Conversion Time = 3 Tad (SAMC) + 12Tad = 1640.625 ns = 609.5 kHz(ksps)
	
	ADCON3 = 0x0000;
	
    ADCON3bits.SAMC = 3; 		// 3 TAD
    ADCON3bits.ADRC = 0; 		// Clock derived from system clock
    ADCON3bits.ADCS = 6; 		// TCY/2 • (ADCS<5:0> + 1) = 109.375ns
    
    //ADCHS
	// |--------------------------------------------------------------------------------------------
    // | CH123NB<1:0>	CH123SB	CH0NB	CH0SB<3:0>	CH123NA<1:0>	CH123SA		CH0NA	CH0SA<3:0>
    // |--------------------------------------------------------------------------------------------
    // | 	00				1	  0		  0101			00			   0		  0	       0101
    // |--------------------------------------------------------------------------------------------
	
	ADCHS = 0x0000;
	
	/* MUX B */
    //ADCHSbits.CH123NB 	= 0b00;		// 0x = CH1, CH2, CH3 negative input is VREF-
    //ADCHSbits.CH123SB 	= 1;		// 1 = CH1 positive input is AN3, CH2 positive input is AN4, CH3 positive input is AN5
    //ADCHSbits.CH0NB		= 0;		// Channel 0 negative input is VREF- 
    //ADCHSbits.CH0SB		= 0b0101;	// SAME Channel 0 positive input is AN5  
    
    /* MUX A */
    //ADCHSbits.CH123NA 	= 0b00;		// 0x = CH1, CH2, CH3 negative input is VREF-
    //ADCHSbits.CH123SA 	= 0;		// 0 = CH1 positive input is AN0, CH2 positive input is AN1, CH3 positive input is AN2
    //ADCHSbits.CH0NA		= 0;		// Channel 0 negative input is VREF-
    //ADCHSbits.CH0SA		= 0b0101;	// Channel 0 positive input is AN5    
    
    //	ADPCFG
	// |--------------------------------------------------------------------------------------------------
    // | x	x	x	x	x	x	x	PCFG8	PCFG7	PCFG6	PCFG5	PCFG4	PCFG3	PCFG2	PCFG1	PCFG0
    // |--------------------------------------------------------------------------------------------------
    // | 1	1	1	1	1	1	1	  1		  1       1       0		  0       0       0       0       0
    // |--------------------------------------------------------------------------------------------------
    
    ADPCFG = 0xFFFF;		// Al pins in digital mode
    
    //ADPCFGbits.PCFG8 = 1;	// ADC_ANALOG_PIN_8_IN_DIGITAL_MODE;
    //ADPCFGbits.PCFG7 = 1;	// ADC_ANALOG_PIN_7_IN_DIGITAL_MODE;
    //ADPCFGbits.PCFG6 = 1;	// ADC_ANALOG_PIN_6_IN_DIGITAL_MODE;
    ADPCFGbits.PCFG5 = 0;	// 0 = Analog input pin in Analog mode, port read input disabled, A/D samples pin voltage
    ADPCFGbits.PCFG4 = 0;	// 0 = Analog input pin in Analog mode, port read input disabled, A/D samples pin voltage
    ADPCFGbits.PCFG3 = 0;	// 0 = Analog input pin in Analog mode, port read input disabled, A/D samples pin voltage
    ADPCFGbits.PCFG2 = 0;	// 0 = Analog input pin in Analog mode, port read input disabled, A/D samples pin voltage
    ADPCFGbits.PCFG1 = 0;	// 0 = Analog input pin in Analog mode, port read input disabled, A/D samples pin voltage
    ADPCFGbits.PCFG0 = 0;	// 0 = Analog input pin in Analog mode, port read input disabled, A/D samples pin voltage
    
    //	ADCSSL
	// |--------------------------------------------------------------------------------------------------
    // | x	x	x	x	x	x	x	CSSL8	CSSL87	CSSL6	CSSL5	CSSL4	CSSL3	CSSL2	CSSL1	CSSL0
    // |--------------------------------------------------------------------------------------------------
    // | 0	0	0	0	0	0	0	  0       0       0       0       0       0       0       0       0
    // |--------------------------------------------------------------------------------------------------
    
    ADCSSL = 0x0000;

    _ADIP = cISR_PRIORITY_ADC;
    
    _ADIF = 0;
    _ADIE = 0;
    			
	ADCON1bits.ADON = 1;		//Enable AD module
}

/******************************************************************************/
/*
* Purpose: ADC_Process AN0 AN1 AN2 AN5. Convert ADC. Call every PWM Interrupt
* Input: none
* Output: none
*/
/******************************************************************************/
void ADC_Process(void)
{
	U16 dummy;
	
	//CONVERT_CH0_CH1_CH2_CH3();	// Convert All SH Channels
	
	/* MUXA First Configuration again*/
	/* AN5 -> CH0 (Uuv) */
	/* AN0 -> CH1 (Iu)  */
	/* AN1 -> CH2 (Iw)  */
	/* AN2 -> CH3 (Uvw) */
	LOAD_MUXA_CFG_FIRST();		// Load First MUXA Configuration
	
	ADCON1bits.SAMP = 1;		// Sample
	
	while(!ADCON1bits.DONE);	// Wait to convert
	ADCON1bits.DONE = 0;
	
	/* Read buffers from First MUXA Configuration */
	ADC[AN_5] = ADCBUF0;	// SH CH0 (Uuv) 
	ADC[AN_0] = ADCBUF1;	// SH CH1 (Iu)	
	ADC[AN_1] = ADCBUF2;	// SH CH2 (Iw)
	ADC[AN_2] = ADCBUF3;	// SH CH3 (Uvw)
	
	//CONVERT_CH0_CH1();			// Convert Only SH: CH0 and CH1
	
	/* MUXA Second Configuration. CH0, CH1 is used. */
	/* AN4 -> CH0 (Idc) 		*/
	/* AN3 -> CH1 (Udc) 		*/
	/* AN2 -> CH2 - Do Not Convert This Channel	*/
	/* AN5 -> CH3 - Do Not Convert This Channel */
	LOAD_MUXA_CFG_SECOND();		// Load Second MUXA Configuration
	
	ADCON1bits.SAMP = 1;		// Sample
	
	while(!ADCON1bits.DONE); 	// Wait to convert
	ADCON1bits.DONE = 0;
	
	/* Read buffers from Second MUXA Configuration */
	ADC[AN_4] = ADCBUF0;		// SH CH0 (Idc)
	ADC[AN_3] = ADCBUF1;		// SH CH1 (Udc)
	dummy = ADCBUF2;
	dummy = ADCBUF3;
	
	_ADIF = 0;
}


