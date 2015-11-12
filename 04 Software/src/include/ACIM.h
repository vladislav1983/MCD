#ifndef ACIM_H 
#define ACIM_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define MOTOR_SPEED_SCALE_CONSTANT		8
/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/* MOTOR INPUT PARAMETERS. VALUES MUST BE IN EEPROM */
typedef struct sMotPar
{
	/* Parameters Stored in EEPROM */
	S16 RPMN_mot;		// in 0.1 RPM Motor Nominal Speed - Parametrized = Motor Name Plate
	S16 FN_mot;			// in 0.1 Hz Motor Supply Voltage Frequency - Parametrized = Motor Name Plate
	S16 UN_mot;			// in 0.1 V Motor Supply Voltage - Parametrized = Motor Name Plate	
	S16 IN_mot;			// in 0.1 A Rated Motor Current - Parametrized = Motor Name Plate
	S16 I0;				// in 0.1 A No load current - Parametrized = (IN_mot + 1.9) / 2.6 -> Empirical defined formula! No physical explanation.
	S16 SigmaLss;		// in 0.1 mH Leakage Factor -  Paramertized = UN_mot/(5.5*IN_mot*omsN*sqrt(3))
	S16 Rs;				// in 1.0 Ohms * 4096 - Stator Resistance - Parametrized = 0.02*UN_mot/(IN_mot-2.0)
	S16 Rr;				// in 1.0 Ohms * 4096 - Rotor Resistance -  Paramertrized = (omsN-omN)(Lm+sigmaLs/2)I0/IN_mot
	S16 Ls;				// in 0.1 mH - Stator Inductance - Paramertized = UN_mot/(I0*omsN*sqrt(3))	
	S16 cosphi;			// in 0.01 cos phi
	S16 TN_mot;			// in 0.1 Nm - Rated Torque
	
	// Precalulated Parameters
	S16 Poles_mot;		// Motor Poles
	S16 omsN;			// Stator Flux Angular Frequency (360° = 2^16)
	S16 omN;			// Rotor Angular Frequency (360° = 2^16)
	S16 T_el;			// in 0.1 Nm - Electrical torcue
	
	// Parameters from Offline identification
	S16 SigmaLss_ID;
	
	
}tsMotPar;



/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
/* GET POINTERS TO STRUCTURES */
extern 	tsMotPar						motpar; 
#define _get_motpar()					(&motpar)

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/



#endif /* ACIM_H */
