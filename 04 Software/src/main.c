#include <p30F6015.h>

/* Configuration Bit Settings */
_FOSC(CSW_FSCM_OFF & XT_PLL16); 
_FWDT(WDT_OFF & WDTPSA_1 & WDTPSB_1); //2ms Watchdog timer under software control.
_FBORPOR(PBOR_ON & BORV_27 & PWRT_64 & MCLR_EN);
//_FGS(CODE_PROT_ON);   // ENABLE CODE PROTECTION
_FGS(CODE_PROT_OFF);

/* User Include */
#include "includes.h"
#include "SysParams.h"
#include "Measure.h"
#include "Control_OpenLoop.h"
/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported functions                     				      				  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/

int main()
{

/*----------------------------------------------------------------------------*/
/*								SYSTEM INIT			                          */
/*----------------------------------------------------------------------------*/	
	_SWDTEN = 0; // Disable WDT
	
	OSCCONbits.COSC = 0b11;
	OSCCONbits.NOSC = 0b11;
	OSCCONbits.CF = 0;
	while(OSCCONbits.LOCK!=1) {;}; // Wait for PLL to lock
	
	SET_CPU_IPL(0b111);			// Force CPU interrupt priority to max.All user masked interrupts with ptiority from 0 to 7 are Disabled.(Safe version)
	 
	delay_ms(5);
/*----------------------------------------------------------------------------*/
/*								MAIN PROGRAM		                          */
/*----------------------------------------------------------------------------*/
	/* INIT */
	DSP_Engine_Init();
	
	/* Parameters Read */
	ee_param_act(1,1); 	// INIT and put in PARACT_PARAMS_READ_ALL state
	ee_param_act(0,0);	// RUN read all parameters. NO NEED REINIT!
	
	/* HW Modules Init */
	IF_DigitalIO_Init();
	IF_PWM_Init();
	IF_DigitalIO_Init();
	IF_SysTmr1_Init();
	IF_SysTmr2_Init();
	IF_SysTmr3_Init();
	IF_Uart_Init(38400);
	IF_ADC_INIT();
	
	/* Software Modules Init */
	Task_Init();
	Task1_Stack_Usage_Calc(_get_sTimes(), 1);
	trace_init_first();
	measure_init();
	VF_ControlInit();
	init_ramp1(0);
	init_ramp2(0);
	

	/* END INIT */
	
	INTCON1bits.NSTDIS = 0;	// Enable Nested Interrupts
	SET_CPU_IPL(0b000);		// Force CPU interrupt priority to low. All user masked interrupts with ptiority from 0 to 7 are Enabled.(Safe version)
	
	
	
	ClrWdt(); 		// Clear WDT
	//_SWDTEN = 1;	// Enable WDT	
	
	while(1)
	{
	/* BACKGROUND LOOP. */
		
		ee_param_act(0,0); // Online parameters actualization in background
		
		Sirem_Engine(); // Serial Communication run in background loop	
		
		adc_par_convert();
		
		TaskTimesCalc(_get_sTimes());
		
		VF_Par_Convert();
		
		
	}

	return(0);
}



