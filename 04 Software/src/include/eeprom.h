#ifndef EEPROM_H
#define EEPROM_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include <libpic30.h>
#include "basedef.h"
#include "PARLIST.h"

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#ifndef  dsPIC30F6015_EEPROM_CONFIGURED
	#error "!!! dsPIC30F6015 EEPROM ADDRESS NOT CONFIGURED, PLEASE CHECK 'EE_BASE_ADDR AND' AND DEFINE THIS ... !!!"
#else

#define EE_BASE_ADDR			0x7FF000 //dsPIC30F6015 eeprom Base Address
#define EE_END_ADDR				0x7FFFFF
#define	EE_SIZE_BYTES			(EE_END_ADDR - EE_BASE_ADDR)
#define	EE_SIZE_WORDS			((EE_SIZE_BYTES / 2))

#define EE_CHKS_ADDR			EE_SIZE_WORDS
#define EE_MAX_WORD_BOUND		EE_SIZE_WORDS - 1

#endif

/* Param Actualisation states */
#define PARACT_INIT				1
#define PARACT_CHECKSUM_POOL	2
#define PARACT_PARAMS_UPDATE	3
#define PARACT_PARAMS_READ_ALL	4

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/
typedef 	U8				HRESULT_EEBUSY;

#define 	EE_BUSY			( (HRESULT_EEBUSY)0x00u )
#define 	EE_NOT_BUSY		( (HRESULT_EEBUSY)0x01u )
/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
struct par_act
	{
	U16 			Iopar_members;
	U16 			Iopar_Index;
	U16 			EE_Chk_act;
	HRESULT_EEBUSY 	EE_busy;
	};
	
extern U16 EE_ERR_REG;
/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/
extern volatile struct par_act paract;
#define _get_paract()	(&paract)
/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
/*	
	EE_ERR_REG 
	Bit 0		-	Checksum Error EEPROM Read		1: Checksum Error read	0: OK	
	Bit 1		-	
	Bit 2		-	EE_VALID						1: Invalid EEPROM Params	0: OK	
	Bit 3		-	EEPROM Write Failure			1: Fault					0: OK
	Bit 4		-	
	Bit 5		-	
	Bit 6		-	
	Bit 7		-	
	Bit 8		-		
	Bit 9		-	
	Bit 10		-	
	Bit 11		-
	Bit 12		-
	Bit 13		-
	Bit 14		-
*/

#define _ee_checksum_read_error()		(_getbit(EE_ERR_REG, 0))  	  	/* bit 0 */
#define _set_ee_checksum_read_error(x)	(_putbit((x), EE_ERR_REG, 0)) 	/* bit 0 */

//#define _ee_attempt_read_long()		(_getbit(EE_ERR_REG, 1))  	  	/* bit 1 */
//#define _set_ee_attempt_read_long(x)	(_putbit((x), EE_ERR_REG, 1)) 	/* bit 1 */

#define _ee_valid_error()				(_getbit(EE_ERR_REG, 2))  	  	/* bit 2 */
#define _set_ee_valid_error(x)			(_putbit((x), EE_ERR_REG, 2)) 	/* bit 2 */

#define _ee_write_error()				(_getbit(EE_ERR_REG, 3))  	   	/* bit 3 */
#define _set_ee_write_error(x)			(_putbit((x), EE_ERR_REG, 3)) 	/* bit 3 */

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void ee_param_act(bool init,bool read_all_params);
HRESULT_EEBUSY EEPROM_busy(void);


#endif /* EEPROM_H */

