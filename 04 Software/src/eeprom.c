/******************************************************************************/
/* 						INTERNAL EEPROM STORAGE ROUTINES
 *                      V2.00
 *						V.Gyurov 23.10.2010        
 *       
 */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef EEPROM_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define EEPROM_C
#endif

/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include <libpic30.h>
#include "includes.h"

/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/
U16 _EEDATA(512) 	EEdata[512];

/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/
volatile U16 ee_state;
volatile struct par_act paract;
/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
U16 EE_CHKS;
U16 EE_Valid;
U16 EE_par_update;
U16 EE_ERR_REG;
/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/
extern const iolist iopar[];

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
void EE_Write_U16(U16 *EEWord, _prog_addressT *EEpointer);
void EE_Erase_U16(_prog_addressT *EEpointer);
void EE_Erase_All(void);
void EE_MemCopy(_prog_addressT *EEpointer,U16 *U16EE_Read_Temp);
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
* Purpose: Parameters actualization. Run in background loop.
* Input: init - initialization flag 1: INIT 0: Normal operation. read_all_params 1: Perform all parameters read from eeprom -> must be combined with init = 1
* Output:  none
*/
/******************************************************************************/
void ee_param_act(bool init,bool read_all_params)
{
	volatile struct par_act *pa;
	static iolist *pio;
	static _prog_addressT EE_pio;
	U16 U16ParamTemp;
	U16 U16ToEEprom;
	U32 U32ParamTemp;
	
	pa = _get_paract();
	
	if(init) ee_state = PARACT_INIT;
	
	switch(ee_state)
	{
	/*------------------------*/
	case PARACT_INIT:
		pio=(iolist*)iopar;
		_init_prog_address(EE_pio, EEdata[0]);
		(pa)->Iopar_members = IF_Parlist_GetCntMembers();
		(pa)->Iopar_Index = 0;
		(pa)->EE_Chk_act = 0;
		
		if(read_all_params) ee_state = PARACT_PARAMS_READ_ALL;
		else ee_state = PARACT_CHECKSUM_POOL;
		
	break;
	/*------------------------*/
	case PARACT_CHECKSUM_POOL:
		if(pio->type[1] == 'n')
		{
			if( sizeof(U32) == pio->size )
			{
				(pa)->EE_Chk_act += (U16)Hiu(*(U32*)pio->read_adress); 
				(pa)->EE_Chk_act += (U16)Lou(*(U32*)pio->read_adress);
			}
			else if(sizeof(U16) == pio->size)
    		{
        		(pa)->EE_Chk_act += *(U16*)pio->read_adress;
    		}
			else if( sizeof(U8) == pio->size )
			{
				(pa)->EE_Chk_act += *(U8*)pio->read_adress;
			}
		}
		
		(pa)->Iopar_Index++;
		
		if((pa)->Iopar_Index >= (pa)->Iopar_members)
		{
			(pa)->Iopar_Index = 0;
			pio=(iolist*)iopar;
			
			if((pa)->EE_Chk_act != EE_CHKS) ee_state = PARACT_PARAMS_UPDATE;
			
			(pa)->EE_Chk_act = 0;
		}
		else pio++;	
	break;
	/*------------------------*/
	case PARACT_PARAMS_UPDATE:
		
		if((pio->type[1] == 'n') && (!_ee_write_error()))
		{
		/**********************************************************************************************************/
			if( sizeof(U32) == pio->size )	// U32 size
			{
				/* Read U32 Param from EEPROM - LOW 16 Bytes FIRST*/
				EE_MemCopy(&EE_pio, &U16ParamTemp);	// Copy LOW byte from EEPROM
				EE_pio += sizeof(U16);				// increment eeprom pointer with 2 bytes to read next bytes from U32 value
				
				U32ParamTemp = (U32)U16ParamTemp;	// store to U32 low 
				
				EE_MemCopy(&EE_pio, &U16ParamTemp);	// Copy High byte from EEPROM
				
				
				U32ParamTemp |=  (U32)U16ParamTemp << 16;	// store U16ParamTemp to high
				
				/* Write U32 param to EEPROM - Little Endian - LOW FIRST */
				if(U32ParamTemp != *(U32*)pio->read_adress)	// If parameter in EEPROM is different from parameter in parlist
				{	
					EE_pio -= sizeof(U16);				// Move back EEPROM pointer to begin of U32 value !!!
					 
					U16ToEEprom = (U16)Lou(*(U32*)pio->read_adress);	// extract low 16 bytes form U32
					
					(pa)->EE_busy = EE_BUSY;
					EE_Erase_U16(&EE_pio);
					EE_Write_U16(&U16ToEEprom, &EE_pio);
					EE_pio += sizeof(U16);	// increment EEPROM pointer
					
					U16ToEEprom = (U16)Hiu(*(U32*)pio->write_adress);	// extract high 16 bytes from U32
					
					EE_Erase_U16(&EE_pio);
					EE_Write_U16(&U16ToEEprom, &EE_pio);	// Write high 16 bytes to eeprom. we do not increment eeprom pointer, because this is done at end of state
					EE_pio -= sizeof(U16);					// Move back EEPROM pointer to begin of U32 value
					(pa)->EE_busy = EE_NOT_BUSY;
					
					EE_par_update++;	// increment debug counter
					
					/* Read U32 Param from EEPROM AGAIN to be sure that written value is there :) - Little Endian - LOW FIRST*/
					EE_MemCopy(&EE_pio, &U16ParamTemp);	// Copy LOW byte from EEPROM
					EE_pio += sizeof(U16);				// increment eeprom pointer with 2 bytes to read next bytes from U32 value
					
					U32ParamTemp = (U32)U16ParamTemp;	// store to U32 low 
					
					EE_MemCopy(&EE_pio, &U16ParamTemp);	// Copy High byte from EEPROM, we do not decrement eeprom pointer, because this is done at end of state
					
					U32ParamTemp |=  (U32)U16ParamTemp << 16;	// store U16ParamTemp to high
					
					if(U32ParamTemp != *(U32*)pio->write_adress)
		    		{
			    		_set_ee_write_error(1);
			    	}
				}
				
				/* Actualize checksum */
			(pa)->EE_Chk_act += (U16)Hiu(*(U32*)pio->read_adress); 
			(pa)->EE_Chk_act += (U16)Lou(*(U32*)pio->read_adress);
					
			}
			else if(sizeof(U16) == pio->size)	// U16 size
    		{
	    		/**********************************************************************************************************/
	    		EE_MemCopy(&EE_pio, &U16ParamTemp);
	    		
	    		if(U16ParamTemp != *(U16*)pio->read_adress)
	    		{
		    		U16ToEEprom = *(U16*)pio->read_adress;
		    		
		    		(pa)->EE_busy = EE_BUSY;
	    			EE_Erase_U16(&EE_pio);
					EE_Write_U16(&U16ToEEprom, &EE_pio);
					(pa)->EE_busy = EE_NOT_BUSY;
					
					EE_par_update++;
					
					EE_MemCopy(&EE_pio, &U16ParamTemp);
		    		if(U16ParamTemp != *(U16*)pio->read_adress)
		    		{
			    		_set_ee_write_error(1);
		    		}
	    		}
	    		
        		(pa)->EE_Chk_act += *(U16*)pio->read_adress;	
    		}
    		else if(sizeof(U8) == pio->size)	// U8 size
    		{
	    		/**********************************************************************************************************/
	    		EE_MemCopy(&EE_pio, &U16ParamTemp);
	    		
	    		if(U16ParamTemp != *(U8*)pio->read_adress)
	    		{
		    		U16ToEEprom = *(U8*)pio->read_adress;
		    		
		    		(pa)->EE_busy = EE_BUSY;
	    			EE_Erase_U16(&EE_pio);
					EE_Write_U16(&U16ToEEprom, &EE_pio);
					(pa)->EE_busy = EE_NOT_BUSY;
					
					EE_par_update++;
					
					EE_MemCopy(&EE_pio, &U16ParamTemp);
		    		if(U16ParamTemp != *(U8*)pio->read_adress) 
		    		{
			    		_set_ee_write_error(1);
			    	}
	    		}

        		(pa)->EE_Chk_act += *(U8*)pio->read_adress;	
    		}
    		
    		EE_pio += sizeof(U16);	// increment EEPROM pointer
    					
		}	// END if(pio->type[1] == 'n')
		
		(pa)->Iopar_Index++;
		
		if((pa)->Iopar_Index >= (pa)->Iopar_members)
		{
			(pa)->Iopar_Index = 0;
			pio=(iolist*)iopar;
			
			EE_CHKS = (pa)->EE_Chk_act;
			
			(pa)->EE_busy = EE_BUSY;
			_init_prog_address(EE_pio, EEdata[EE_CHKS_ADDR]);
			EE_Erase_U16(&EE_pio);
			EE_Write_U16(&EE_CHKS, &EE_pio);
			(pa)->EE_busy = EE_NOT_BUSY;
			
			EE_par_update++;
			
			EE_MemCopy(&EE_pio, &U16ParamTemp);
	    	if(U16ParamTemp != EE_CHKS)
	    	{
		    	_set_ee_write_error(1);
		    }
			
			_init_prog_address(EE_pio, EEdata[0]);
			
			ee_state = PARACT_INIT;
			
		}
		else pio++;
	break;
	/*------------------------*/
	case PARACT_PARAMS_READ_ALL:
		(pa)->EE_busy = EE_BUSY;
		
		for((pa)->Iopar_Index = 0; (pa)->Iopar_Index < (pa)->Iopar_members; (pa)->Iopar_Index++)
		{
			if(pio->type[1] == 'n')
			{
				if( sizeof(U32) == pio->size )
				{
					/* Read U32 Param from EEPROM - Little Endian - LOW FIRST*/
					EE_MemCopy(&EE_pio, &U16ParamTemp);	// Copy LOW byte from EEPROM
					EE_pio += sizeof(U16);				// increment eeprim pointer with 2 bytes to read next bytes from U32 value
					(pa)->EE_Chk_act += U16ParamTemp;	// update checksum
					
					U32ParamTemp = (U32)U16ParamTemp;	// store to U32 low 
					
					EE_MemCopy(&EE_pio, &U16ParamTemp);	// Copy High byte from EEPROM, we do not increment eeprom pointer, because this is done at end of cycle
					(pa)->EE_Chk_act += U16ParamTemp;	// update checksum
					
					U32ParamTemp |=  (U32)U16ParamTemp << 16;	// store U16ParamTemp to high
					
					*(U32*)pio->write_adress = U32ParamTemp;
				}
				else if( sizeof(U16) == pio->size )
				{
					EE_MemCopy(&EE_pio, &U16ParamTemp);
			
					*(U16*)pio->write_adress = U16ParamTemp;
			
					(pa)->EE_Chk_act += U16ParamTemp;
				}
				else if( sizeof(U8) == pio->size )
				{
					EE_MemCopy(&EE_pio, &U16ParamTemp);
				
					*(U8*)pio->write_adress = U16ParamTemp;
				
					(pa)->EE_Chk_act += U16ParamTemp;
	
				}
			
				EE_pio += sizeof(U16);	// increment eeprom pointer
			
			}//end if(pio->type[1] == 'n')
			pio++;
		}//end for
	
		/* Checksum Read */
		_init_prog_address(EE_pio, EEdata[EE_CHKS_ADDR]);
		EE_MemCopy(&EE_pio, &EE_CHKS);
	
		if((pa)->EE_Chk_act != EE_CHKS) _set_ee_checksum_read_error(1);
		if(EE_Valid != 1000) _set_ee_valid_error(1);
		
		ee_state = PARACT_INIT;
		(pa)->EE_busy = EE_NOT_BUSY;

	break;
	/*------------------------*/
		default:
		Nop();
		break;
	
	}
}
/******************************************************************************/
/*
 * Purpose: Get State of EEPROM
 * Inputs:  none
 * Output:  S_OK - EEprom Busy, S_FALSE - EEPROM NOT BUSY
 */
/******************************************************************************/
HRESULT_EEBUSY EEPROM_busy(void)
{
		
	return(paract.EE_busy);
	
}

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*
 * Name:      EEwrite
 * Purpose:   Write a WORD! into a eeprom
 */
/******************************************************************************/
void EE_Write_U16(U16 *EEWord, _prog_addressT *EEpointer)
{
	
	_write_eedata_word(*EEpointer, *EEWord);
	_wait_eedata();
			

}
/******************************************************************************/
/*
 * Name:      
 * Purpose:   Erase all EEprom
 */
/******************************************************************************/
void EE_Erase_All(void)
{
	
	_erase_eedata_all();
	_wait_eedata();
	

}
/******************************************************************************/
/*
 * Name:      
 * Purpose:   Erase a fixed word
 */
/******************************************************************************/
void EE_Erase_U16(_prog_addressT *EEpointer)
{
	
	
	_erase_eedata(*EEpointer, _EE_WORD);
	_wait_eedata();	
	

}
/******************************************************************************/
/*
 * Name:      EE_MemCopy  
 * Purpose:   Read U16 (WORD) value from EEprom - EEpointer
 */
/******************************************************************************/
void EE_MemCopy(_prog_addressT *EEpointer,U16 *U16EE_Read_Temp)
{
	
	
	(void) _memcpy_p2d16(U16EE_Read_Temp, *EEpointer, _EE_WORD);	//High first

	//return ((U16*)U16EE_Read_Temp);
}


