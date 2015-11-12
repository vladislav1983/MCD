#ifndef SYSPARAMS_H 
#define SYSPARAMS_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                             */
/*----------------------------------------------------------------------------*/
/* Inverter electrical parameters. Defined in RefGen.h */
struct sInv_Params_Struct
{
	U16 Line_Voltage;			// Line Voltage [V]
	U16 DC_Link_Voltage_max;	// Rated DC-Link Voltage [V]
	U16 DC_Link_Voltage_min;	// Minimum DC-Link Voltage [V]
	U16 DC_Link_Current_max;	// Rated DC-Link Current [A]
	U16 Inv_Voltage_max;		// Rated Inverter Voltage [V]
	U16 Inv_Current_max;		// Rated Inverter Current [A]
	
	U16 Inv_Fs_Max;				// in 0.1 Hz - Inverter Max Frequency
	U16 Inv_Fs_Min;				// in 0.1 Hz - Inverter Min Frequency
	
	S16 I_norm;					// in 0.1 A - Normalized current
	
};

extern struct sInv_Params_Struct 		sInv_Params;
#define _get_sInv_Params()				(&sInv_Params)

/* Trip Values Structure. Defined in Measure.h */
struct sTrip_Levels_Struct
{
	U16 DC_Link_Trip_V_max;			// max DC Link Voltage Trip [V]
	U16 DC_Link_Trip_V_min;			// min DC Link Voltage Trip [V]
	U16 DC_Link_Current_Trip_A;		// DC Link Current Trip [A]
	U16 Inv_Voltage_Trip_V;			// inverter Voltage Trip [V]
	U16 Inv_Current_Trip_A;			// Minverter Current Trip [A]	
};

extern struct sTrip_Levels_Struct sTrip_Levels;

struct sGain_Correction
{
	/* Correction Coefficients in Percents x 10 */
	U16 Iu_corr;	
	U16 Iw_corr;	
	U16 Uuv_corr;
	U16 Uvw_corr;
	U16 Udc_corr;
	U16 Idc_corr;
	
};

extern struct sGain_Correction		sGain_Corr;


/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/



#endif /* SYSPARAMS_H */
