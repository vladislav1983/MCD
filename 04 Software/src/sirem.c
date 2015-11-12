/******************************************************************************/
/* 
 *                      SIREM PROTOCOL IMPLEMENTATION
 *					  V2.0 Zero Wait State Sirem Engine
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef SIREM_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define SIREM_C
#endif
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include <string.h>
#include "includes.h"

#include "sirem.h"
#include "parlist.h"
#include "vuart.h"
#include "trace.h"

/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/
#define SIREM_COMMAND_BYTE		au8SiremFrame[cSiremIndex_Cmd]	
#define _get_au8SiremFrame		&au8SiremFrame[0]
/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/

typedef U8 	 HRESULT_SIREM_STATE;

/* Main Sirem State Machine States */
typedef enum eSirem_Engine_States
{
	/* Protocol Defined States */
	SIREM_GET_VALID_FRAME			= 0x00,
	SIREM_READ_PRM_INFO 			= cSiremCmd_ReadPrmInfo,
	SIREM_READ_PRM_LIST 			= cSiremCmd_ReadPrmList,
	SIREM_READ_PRM 					= cSiremCmd_ReadPrm,
	SIREM_WRITE_PRM					= cSiremCmd_WritePrm,
	SIREM_READ_TRACE_DATA			= cSiremCmd_ReadTrace,
	SIREM_RESTART_DRIVE				= cSiremCmd_Restart,
	
}teSirem_Engine_States;

/* Transmit Struct -> Transmit buffer pointer, and size */
typedef struct sTransmit
{
	U8  *pTrmBuff;
	U16 TrmBuffSize;

}tsTransmit;

/* READ PRM INFO STATES */
typedef enum eSirem_Read_Prm_Info_States
{
	READ_PRM_INFO_MSG_COMPOSE = 0x00,
	READ_PRM_INFO_TRMT_FRAME,

}teSirem_Read_Prm_Info_States;

/* READ PRM LIST STATES */
typedef enum eSirem_Read_Prm_List_States
{
	READ_PRM_LIST_MSG_COMPOSE = 0x00,
	READ_PRM_LIST_SEND_FRAME,
	READ_PRM_LIST_PARAM_COMPOSE,
	READ_PRM_LIST_SEND_PARAM_STRUCT,
	READ_PRM_LIST_PARAM_NAMES_COMPOSE,
	READ_PRM_LIST_SEND_PARAM_NAME,
	READ_PRM_LIST_CHECKSUM_COMPOSE,
	READ_PRM_LIST_CHECKSUM_SEND,
	
}teSirem_Read_Prm_List_States;

/* READ PRM STATES */
typedef enum eSirem_Read_Prm_States
{
	READ_PRM_MSG_COMPOSE = 0x00,
	READ_PRM_TRMT_FRAME,
	
}teSirem_Read_Prm_States;

/* WRITE PRM STATES */
typedef enum eSirem_Write_Prm_States
{
	WRITE_PRM_GET_PRM = 0x00,
	WRITE_PRM_TRMT_FRAME,
	
}teSirem_Write_Prm_States;

/* READ TRACE DATA STATES */
typedef enum eSirem_Read_Trace_States
{
	READ_TRACE_COMPOSE_FRAME = 0x00,
	READ_TRACE_TRMT_FRAME,
	READ_TRACE_ARRAY_HEADER_COMPOSE,
	READ_TRACE_TRMT_TRACE_ARRAY_HEADER,
	READ_TRACE_TRMT_TRACE_ARRAY,
	
}teSirem_Read_Trace_States;

/* RESTART DRIVE STATES */
typedef enum eSirem_Restart_Drive_States
{
	RESTART_DRIVE_CONFIRM = 0x00,
	RESTART_DRIVE_TRMT_FRAME,
	RESTART_DRIVE_RESTART,
	
}teSirem_Restart_Drive_States;

/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/
static 	U8 						au8SiremFrame[cSiremFrameSize];   // cUart_RcvBuffSize should be bigger.
static  tsTransmit				trmt;
static  teSirem_Engine_States   siremstate;

/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
extern const iolist iopar[];
extern U16 release;    		
extern U32 version;
extern S16 TRSB[TRACE_STEPS*NB_TRACE_VARS];

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
static HRESULT_SIREM_STATE 	GetValidSiremFrame	(U8* pFrame);
static HRESULT 				CmdExe_ReadPrmInfo	(U8* pFrame);
static HRESULT 				CmdExe_ReadPrmList	(U8* pFrame);
static HRESULT 				CmdExe_ReadPrm		(U8* pFrame);
static HRESULT 				CmdExe_WritePrm		(U8* pFrame);
static HRESULT 				CmdExe_ReadTraceData(U8* pFrame);
static HRESULT 				CmdExe_Reset		(U8* pFrame);

static inline U16 			Sirem_Checksum_Calc	(U8 *pBuff, U16 BuffSize);
static void 				Put_In_TRM_Buff		(U8 *pBuff, U16 BuffSize);
static HRESULT 				Transmit_Sirem_Buffer(void);

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*
 * Purpose: Sirem Engine. Run In Background Loop
 * Inputs:  none
 * Output:  none
 */
/******************************************************************************/
void Sirem_Engine(void)
{
	
	switch(siremstate)
	{
	/**************************************************************************/
	/*
	 * Purpose: Block in this state until valid Sirem frame is received
	 */
	/**************************************************************************/
	case SIREM_GET_VALID_FRAME:
	
		siremstate = GetValidSiremFrame(au8SiremFrame);
		
	break;
	/**************************************************************************/
	/*
	 * Purpose: get size and number of iolist member and send via rs232
	 */
	/**************************************************************************/
	case SIREM_READ_PRM_INFO:
		
		if( CmdExe_ReadPrmInfo(au8SiremFrame) == S_OK)
		{
			siremstate = SIREM_GET_VALID_FRAME;
		}
		
	break;
	
	/**************************************************************************/
	/*
	 * Purpose: read param list
	 */
	/**************************************************************************/
	case SIREM_READ_PRM_LIST:
		
		if( CmdExe_ReadPrmList(au8SiremFrame) == S_OK )
		{
			siremstate = SIREM_GET_VALID_FRAME;
		}
	    
	break;
	
	/**************************************************************************/
	/*
	 * Purpose: Parameter Read
	 */
	/**************************************************************************/
	case SIREM_READ_PRM:
		
		if( CmdExe_ReadPrm(au8SiremFrame) == S_OK )
		{
			siremstate = SIREM_GET_VALID_FRAME;
		}
		
	break;
	
	/**************************************************************************/
	/*
	 * Purpose: Parameter Write
	 */
	/**************************************************************************/
	case SIREM_WRITE_PRM:
		
		if( CmdExe_WritePrm(au8SiremFrame) == S_OK )
		{
			siremstate = SIREM_GET_VALID_FRAME;
		}
	    
	break;
	
	/**************************************************************************/
	/*
	 * Purpose: read trace data array
	 */
	/**************************************************************************/
	case SIREM_READ_TRACE_DATA:
	
		if( CmdExe_ReadTraceData(au8SiremFrame) == S_OK )
		{
			siremstate = SIREM_GET_VALID_FRAME;
		}
		
	    
	break;
	
	/**************************************************************************/
	/*
	 * Purpose: Drive Restart Confirm and Restart
	 */
	/**************************************************************************/
	case SIREM_RESTART_DRIVE:
		
		if( CmdExe_Reset(au8SiremFrame) == S_OK )
		{
			siremstate = SIREM_GET_VALID_FRAME;
		}

	break;
	
	/**************************************************************************/
	/*
	 * Purpose: Default State Handler
	 */
	/**************************************************************************/	
	default:
	
		siremstate = SIREM_GET_VALID_FRAME;
		
	break;
	/*-----------------------------------------------------------------------*/
	
	
	}// end switch
	
}

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                       L O C A L   F U N C T I O N S                        */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*
 * Purpose: Block here until valid Sirem frame is received
 * Inputs:  none
 * Output:  Extracted SIREM Command
 */
/******************************************************************************/
static HRESULT_SIREM_STATE GetValidSiremFrame(U8* pFrame) //return pointer *pframe
{
    U8 u8Temp;
    U8 u8Chks;
    U8 sirem_state;

    u8Temp = IF_Uart_MsgGetDataCnt();

    if( 0==u8Temp )	//if data count =0
    {
        // No frame received;
        return(0);
    }
    else if( cSiremFrameSize==u8Temp )	//sirem size = data count
    {
        // It is possible to be Sirem frame.
        IF_Uart_MsgGet(pFrame);	//get msg

        u8Chks = (U8)Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks);
        
		//~u8Chks is checksum. (~u8Chks == 9u) AND ((cSiremValueFrameStart=0u) == (pFrame[cSiremIndex_FrameStart]))
        if( ((U8)(~u8Chks) == pFrame[cSiremIndex_Chks]) && (cSiremValueFrameStart==pFrame[cSiremIndex_FrameStart]) )
        {
            sirem_state = au8SiremFrame[cSiremIndex_Cmd];
            return(sirem_state);
        }

    }
    else
    {
        // Unknown frame
    	IF_Uart_RxPurge();	//clean input rx structure
    }
    
    return(0);
}

/******************************************************************************/
/*
 * Purpose: get size and number of iolist member and send via rs232
 * Inputs:  pointer of sirem frame
 * Output:  ack status
 */
/******************************************************************************/
static HRESULT CmdExe_ReadPrmInfo(U8* pFrame)
{
	static 	teSirem_Read_Prm_Info_States	prminfo_state;
	
    U32 u32Lenght;  	// Lenght of whole ReadPrmList frame.
    static U16 u16Chks = 0;;	// checksum
    U16 u16Index;		// index
    U16 u16ParamCnt;	//parameter count
    
    switch(prminfo_state)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Compose message frame, calc checksum and put in trmt buffer
	 	*/
		/**********************************************************************/
  		case READ_PRM_INFO_MSG_COMPOSE:
  			
  			// ReadPrmList commands transmitted byte calculation
		    u32Lenght = (1+1+4);                        // add size of "start field", "functional field", "SoftVersion field"
		    u32Lenght += (U32)IF_Parlist_GetCntBytes(); // add size of whole parameters struct.
		    u16ParamCnt = IF_Parlist_GetCntMembers();	//get nimber of iolist members
		    for( u16Index=0; u16Index<u16ParamCnt; u16Index++ )
		    {                                           // add size for each "parameter name" including NULL terminator.
		        u32Lenght += 1 + strlen(iopar[u16Index].name);     // including NULL termination character
		    }
		    u32Lenght += 2;                             // add size of CRC field.
		
		    // fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadPrmInfo;
		    pFrame[2]                       = cSiremValueLowFirst;              // low byte first;
		    pFrame[3]                       = sizeof(iopar[0].name);            // sizeof of corresponding pointers
		    pFrame[4]                       = sizeof(iopar[0].read_adress);
		    pFrame[5]                       = (U8)(u32Lenght>>24);
		    pFrame[6]                       = (U8)(u32Lenght>>16);
		    pFrame[7]                       = (U8)(u32Lenght>>8);
		    pFrame[8]                       = (U8)(u32Lenght>>0);   // according above defined Endianes
		    
		    // Calculate Chacksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks] = (U8)(~u16Chks);
			
			// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
			
			// GOTO transmit state
		    prminfo_state = READ_PRM_INFO_TRMT_FRAME;

    	break;    	
		/**********************************************************************/
		/*
	 	* Purpose: Transmit Frame
	 	*/
		/**********************************************************************/
  		case READ_PRM_INFO_TRMT_FRAME:
  			
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			prminfo_state = READ_PRM_INFO_MSG_COMPOSE;
	  			
	  			return(S_OK);
  			}
  			
    	break;
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			prminfo_state = READ_PRM_INFO_MSG_COMPOSE;
		break;
    } // end switch
    
    return(S_FALSE);  
}
/******************************************************************************/
/*
 * Purpose: read param list. Send All parlist structure with parnames to ID.
 * Inputs:  sirem pframe
 * Output:  ack status. S_OK - Routine finished, S_FALSE - Routine busy.
 */
/******************************************************************************/
static HRESULT CmdExe_ReadPrmList(U8* pFrame)
{
	static teSirem_Read_Prm_List_States rd_prm_list_state;
	
    static U16 u16Chks;
    static U16 u16Index;
    U16 u16BuffSize; 
    U8* pu8Buff;
    static U16 u16ParamCnt;
    
    switch(rd_prm_list_state)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Compose message frame, calc checksum and put in trmt buffer
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_MSG_COMPOSE:
  			
  			//Send - Start, Cmd, SoftVer
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadPrmList;
		    pFrame[2]                       = (U8)(cSoftwareVersion>>0);   // Little Endian (low first)
		    pFrame[3]                       = (U8)(cSoftwareVersion>>8);
		    pFrame[4]                       = (U8)(cSoftwareVersion>>16);
		    pFrame[5]                       = (U8)(cSoftwareVersion>>24);
		    
		    u16Chks = 0;
		    
		   	// Calculate Chacksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, 6); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks] = (U8)(~u16Chks);
			
			// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, 6);
			
		    rd_prm_list_state = READ_PRM_LIST_SEND_FRAME;
		    
    	break;    
		/**********************************************************************/
		/*
	 	* Purpose: Transmit Frame
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_SEND_FRAME:
  			
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_prm_list_state = READ_PRM_LIST_PARAM_COMPOSE;
  			}
  			
    	break;   
		/**********************************************************************/
		/*
	 	* Purpose: Prepare to send iopar structure to send
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_PARAM_COMPOSE:
  			
  			// Send - whole param struct
		    pu8Buff = (U8*)iopar;	//8bit pointer to iopar
		    
		    u16BuffSize = IF_Parlist_GetCntBytes();//get iopar size
		    
		    u16Chks += Sirem_Checksum_Calc(pu8Buff, u16BuffSize); 
		    
		    // Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pu8Buff, u16BuffSize);
		    
		    rd_prm_list_state = READ_PRM_LIST_SEND_PARAM_STRUCT;
		      
    	break;	
		/**********************************************************************/
		/*
	 	* Purpose: Send all iopar structure members
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_SEND_PARAM_STRUCT:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_prm_list_state = READ_PRM_LIST_PARAM_NAMES_COMPOSE;
	  			u16Index = 0;
	  			u16ParamCnt = IF_Parlist_GetCntMembers();	//get number of members on structure iopar
  			}
    	break;   
    	/**********************************************************************/
		/*
	 	* Purpose: Prepare to send all iopar.names
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_PARAM_NAMES_COMPOSE:
  		
  			// Send line by line all param names from iopar[] (including last delimiter line with number 0)
		    if( u16Index < u16ParamCnt )
		    {
		        // Send - Param name string
		        pu8Buff     = (U8*)(iopar[u16Index].name);
		        u16BuffSize = 1 + strlen((char*)pu8Buff);                  // including NULL termination character
		        
		        u16Chks += Sirem_Checksum_Calc(pu8Buff, u16BuffSize);
		        
		    	// Put Buffer In transmit Buffer
		    	Put_In_TRM_Buff(pu8Buff, u16BuffSize);
		    	
		    	rd_prm_list_state = READ_PRM_LIST_SEND_PARAM_NAME;
		    	
		    	u16Index++;
		    }
		    else
		    {
			    rd_prm_list_state = READ_PRM_LIST_CHECKSUM_COMPOSE;
		    }
  			
    	break; 
		/**********************************************************************/
		/*
	 	* Purpose: Send all iopar.names
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_SEND_PARAM_NAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_prm_list_state = READ_PRM_LIST_PARAM_NAMES_COMPOSE;
  			}
  			
    	break;
    	  
		/**********************************************************************/
		/*
	 	* Purpose: Compose checksum frame.
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_CHECKSUM_COMPOSE:
  		
  		 	pFrame[0] = (U8)(u16Chks>>0);               // Little endian
    		pFrame[1] = (U8)(u16Chks>>8);
    		
    		// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, 2);
		    
		    rd_prm_list_state = READ_PRM_LIST_CHECKSUM_SEND;
    		
    	break;
    	
    	/**********************************************************************/
		/*
	 	* Purpose: Send Checksum Frame
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_CHECKSUM_SEND:
  		  	if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_prm_list_state = READ_PRM_LIST_MSG_COMPOSE;
	  			
	  			return(S_OK);
  			}
    	break;
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			rd_prm_list_state = READ_PRM_LIST_MSG_COMPOSE;
		break;
    } // end switch
    
    
    return(S_FALSE);
}

/******************************************************************************/
/*
 * Purpose: Read Parameter. Controller -> ID.
 * Inputs:  pFrame - Sirem Buffer Frame
 * Output:  Ack status. S_OK - Routine finished. S_FALSE -> Routine busy.
 */
/******************************************************************************/
static HRESULT CmdExe_ReadPrm(U8* pFrame)
{
	static teSirem_Read_Prm_States rd_prm_state;
	
    U32 u32PrmValue;
    U16 u16PrmNumber;
    U16 u16Chks = 0;
    
    iolist *pio;
	
	switch(rd_prm_state)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Extract Parameter number and compose message frame.
	 	*/
		/**********************************************************************/
  		case READ_PRM_MSG_COMPOSE:
  		
  			u16PrmNumber = 0x100u * ((U16)pFrame[3]) + pFrame[4];       // Extract Parameter number
  			
		    pio = IF_Parlist_bnu(u16PrmNumber);
		    
		    if( sizeof(U16) == pio->size )                              // Read Parameter value
		    {
		        u32PrmValue = *((U16*)pio->read_adress);                    // 2 bytes parameter value
		    }
		    else if( sizeof(U32) == pio->size )
		    {
		        u32PrmValue = *((U32*)pio->read_adress);                    // 4 bytes parameter value
		    }
		    else
		    {
		        u32PrmValue = *((U8*)pio->read_adress);                     // 1 byte parameter value
		    }
		    
		    // fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadPrm;
		    pFrame[2]                       = 0; // ???
		    pFrame[3]                       = pFrame[3];                // PrmNumber - keep the same
		    pFrame[4]                       = pFrame[4];
		    pFrame[5]                       = (U8)(u32PrmValue>>24);
		    pFrame[6]                       = (U8)(u32PrmValue>>16);
		    pFrame[7]                       = (U8)(u32PrmValue>>8);
		    pFrame[8]                       = (U8)(u32PrmValue>>0);   // according above defined Endianes
  			
  			// Calculate Chacksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks] = (U8)(~u16Chks);
			
			// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
			
		    rd_prm_state = READ_PRM_TRMT_FRAME;
		    	    
    	break; 
    	
    	/**********************************************************************/
		/*
	 	* Purpose: Transmit Message Frame.
	 	*/
		/**********************************************************************/
  		case READ_PRM_TRMT_FRAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_prm_state = READ_PRM_MSG_COMPOSE;
	  			
	  			return(S_OK);
  			}		    
    	break;       
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			rd_prm_state = READ_PRM_MSG_COMPOSE;
		break;    	
    } // end switch
    
    return(S_FALSE);
}

/******************************************************************************/
/*
 * Purpose: Write Parameter Number. Controller <- ID.
 * Inputs:  pFrame - Sirem buffer frame.
 * Output:  Ack Status. S_OK - Routine finished. S_FALSE - Routine busy.
 */
/******************************************************************************/
static HRESULT CmdExe_WritePrm(U8* pFrame)
{
	static teSirem_Write_Prm_States wr_prm_state;
	
    U32 u32PrmValue;
    U16 u16PrmNumber;
    U16 u16Chks = 0;
    iolist *pio;
    
    switch(wr_prm_state)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Get parameter and compose message frame.
	 	*/
		/**********************************************************************/
  		case WRITE_PRM_GET_PRM:
  			
  			
  			u16PrmNumber = 0x100u * ((U16)pFrame[3]) + pFrame[4];       // Extract Parameter number
		
		    u32PrmValue = pFrame[5];                                    // Extract new Parameter value
		    u32PrmValue = (u32PrmValue<<8) + pFrame[6];
		    u32PrmValue = (u32PrmValue<<8) + pFrame[7];
		    u32PrmValue = (u32PrmValue<<8) + pFrame[8];
		
		    pio = IF_Parlist_bnu(u16PrmNumber);
		    
		    if( sizeof(U16) == pio->size )                              // Write new Parameter value
		    {
		        *((U16*)pio->write_adress) = (U16)u32PrmValue;               // 2 bytes parameter value
		    }
		    else if( sizeof(U32) == pio->size )
		    {
		        *((U32*)pio->write_adress) = (U32)u32PrmValue;               // 4 bytes parameter value
		    }
		    else
		    {
		        *((U8*)pio->write_adress) = (U8)u32PrmValue;                 // 1 byte parameter value
		    }
		
		    // fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_WritePrm;
		    pFrame[2]                       = 0; // ???
		    pFrame[3]                       = pFrame[3];                // PrmNumber - keep the same
		    pFrame[4]                       = pFrame[4];
		    pFrame[5]                       = pFrame[5];                // PrmValue  - keep the same
		    pFrame[6]                       = pFrame[6];
		    pFrame[7]                       = pFrame[7];
		    pFrame[8]                       = pFrame[8];
		    
  			// Calculate Chacksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks]        = (U8)(~u16Chks);
		    
		    // Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
			
			// Go to transmit frame state
		    wr_prm_state = WRITE_PRM_TRMT_FRAME;
		    
    	break; 
    	
    	/**********************************************************************/
		/*
	 	* Purpose: Transmit message frame.
	 	*/
		/**********************************************************************/
  		case WRITE_PRM_TRMT_FRAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			wr_prm_state = WRITE_PRM_GET_PRM;
	  			
	  			return(S_OK);
  			}		    
    	break;       
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			wr_prm_state = WRITE_PRM_GET_PRM;
		break;    	
    } // end switch

    return(S_FALSE);
}

/******************************************************************************/
/*
 * Purpose: read trace data array
 * Inputs:  sirem pframe
 * Output:  Ack Status. S_OK - Routine finished. S_FALSE - Routine busy.
 */
/******************************************************************************/
static HRESULT CmdExe_ReadTraceData(U8* pFrame)
{
	static teSirem_Read_Trace_States rd_trace_state;

	static U16 u16Array_length;
	static U32 u32Chks;
	
	static U16 u16Index;
	U16 u16_store_pos;
	U16 u16_nb_var;
	U16 u16_nb_steps;
	
	
	switch(rd_trace_state)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Get command. of array length = 0 -> array information - cars, steps...
	 	* else send whole trace array.
	 	*/
		/**********************************************************************/
  		case READ_TRACE_COMPOSE_FRAME:
  			
  			u16_nb_var = NB_TRACE_VARS;	//channels
			u16_nb_steps = TRACE_STEPS;	//samples
			u16_store_pos = TRACE_STEPS;		//trigger_sample
			
			/* length (number of words to read) */
			u16Array_length = (pFrame[8] + ((U16)pFrame[7]<<8) + ((U32)pFrame[6]<<16));
		
			/* index of the first element to be read */
			u16Index = (((U16)pFrame[5]) + ((U16)pFrame[4]<<8) + ((U32)pFrame[3]<<16));
		
			if(u16Array_length == 0)	/* length = 0: return trace info */
			{
				pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
			    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadTrace;
				pFrame[2] = 0;
				pFrame[3] = (U8)u16_store_pos>>8;
				pFrame[4] = (U8)u16_store_pos;
				pFrame[5] = (U8)u16_nb_var>>8;
				pFrame[6] = (U8)u16_nb_var;
				pFrame[7] = (U8)u16_nb_steps>>8;	
				pFrame[8] = (U8)u16_nb_steps;
			    
	  			// Calculate Chacksum
			    u32Chks = (U16)Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
			    
			    // Invert Checksum
			    pFrame[cSiremIndex_Chks]        = (U8)(~u32Chks);
			    
			    // Put Buffer In transmit Buffer
			    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
				
				// Go to transmit frame state
			    rd_trace_state = READ_TRACE_TRMT_FRAME;
			} 
			else
			{
				rd_trace_state = READ_TRACE_ARRAY_HEADER_COMPOSE;
			}
    	break; 
    	/**********************************************************************/
		/*
	 	* Purpose: Transmit message frame
	 	*/
		/**********************************************************************/
  		case READ_TRACE_TRMT_FRAME:
  		  	if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_trace_state = READ_TRACE_COMPOSE_FRAME;
	  			return(S_OK);
  			}		 
  		break;
  		/**********************************************************************/
		/*
	 	* Purpose: Compose trace array header frame.
	 	*/
		/**********************************************************************/
  		case READ_TRACE_ARRAY_HEADER_COMPOSE:
  		
  			pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadTrace;
		    
			u32Chks = 0;
			u32Chks = (U16)(pFrame[0]<<8) + (pFrame[1]);
			
			// Put Buffer In transmit Buffer
			Put_In_TRM_Buff(pFrame, 2);
				
			// Go to transmit frame state
			rd_trace_state = READ_TRACE_TRMT_TRACE_ARRAY_HEADER;
			
  		break;
  		/**********************************************************************/
		/*
	 	* Purpose: Transmit Trace Array header.
	 	*/
		/**********************************************************************/
  		case READ_TRACE_TRMT_TRACE_ARRAY_HEADER:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			rd_trace_state = READ_TRACE_TRMT_TRACE_ARRAY;
  			}
  		break;
  		/**********************************************************************/
		/*
	 	* Purpose: Transmit Trace Array
	 	*/
		/**********************************************************************/
  		case READ_TRACE_TRMT_TRACE_ARRAY:
			if(u16Array_length > 0)
			{
				u32Chks += TRSB[u16Index];
				
				while( IF_UART_PutByte( (U8)(TRSB[u16Index] >> 0) ) == 0 ) {};
				while( IF_UART_PutByte( (U8)(TRSB[u16Index] >> 8) ) == 0 ) {};
				
				u16Index++;	
				u16Array_length--;
			}
			else
			{
				while( IF_UART_PutByte( (U8)(u32Chks >> 0) ) == 0 ) {};
				while( IF_UART_PutByte( (U8)(u32Chks >> 8) ) == 0 ) {};
				rd_trace_state = READ_TRACE_COMPOSE_FRAME;
				return(S_OK);
			}
  		break;
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			rd_trace_state = READ_TRACE_COMPOSE_FRAME;
		break;    
    	
    }// end switch
    
	return(S_FALSE);
}

/******************************************************************************/
/*
 * Purpose: Restart Controller. 
 * Inputs:  Sirrem Frame pointer
 * Output:  ACK Status.
 */
/******************************************************************************/
static HRESULT CmdExe_Reset(U8* pFrame)
{
	static teSirem_Restart_Drive_States restart_state;

    
    switch(restart_state)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Compose restart confirm frame
	 	*/
		/**********************************************************************/
  		case RESTART_DRIVE_CONFIRM:
  			
  			if(EEPROM_busy() == EE_NOT_BUSY)
  			{
	  			// fill acknowledge frame.
			    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
			    pFrame[cSiremIndex_Cmd]         = cSiremCmd_Restart;
			    pFrame[2]                       = 0; // ???
			    pFrame[3]                       = 0;
			    pFrame[4]                       = 0;
			    pFrame[5]                       = 0;
			    pFrame[6]                       = 0;
			    pFrame[7]                       = 0;
			    pFrame[8]                       = 0;
			    pFrame[cSiremIndex_Chks]		= 0xFF;	// Manulal calculated checksum
			    
			   	Put_In_TRM_Buff(pFrame, cSiremFrameSize);
					
				// Go to transmit frame state
				restart_state = RESTART_DRIVE_TRMT_FRAME;
			}
			else
			{
				return(S_OK);
			}
    	break; 
    	/**********************************************************************/
		/*
	 	* Purpose: Transmit frame.
	 	*/
		/**********************************************************************/
  		case RESTART_DRIVE_TRMT_FRAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			restart_state = RESTART_DRIVE_RESTART;
  			}	
    	break; 
    	/**********************************************************************/
		/*
	 	* Purpose: Restart Drive.
	 	*/
		/**********************************************************************/
  		case RESTART_DRIVE_RESTART:
  			
  			/* RESET CPU */
  			asm("goto __resetPRI"); //changed from goto __reset to be > C30 V3.21 compatible
  			
  			// CPU Restarted here ... who cares about fucking state.
	    
    	break;       
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			restart_state = RESTART_DRIVE_CONFIRM;
		break;    	
    } // end switch
    
    return(S_FALSE);
}

/******************************************************************************/
/*
 * Purpose: Calculate Sirem Checksum.
 * Inputs:  pBuff - poiner to frame. Buffsize - Frame size.
 * Output:  U16 checksum
 */
/******************************************************************************/
static inline U16 Sirem_Checksum_Calc(U8 *pBuff, U16 BuffSize)
{
    U16 i;
    U16 u8Chks = 0;

    for( i=0; i<BuffSize; i++ )
    {
        u8Chks += pBuff[i];
    }

    return(u8Chks);
}

/******************************************************************************/
/*
 * Purpose: Point to transmit buffer
 * Inputs:  Buffer pointer, Buffersize.
 * Output:  none
 */
/******************************************************************************/
static void Put_In_TRM_Buff(U8 *pBuff, U16 BuffSize)
{
	trmt.pTrmBuff 		= (U8 *)pBuff;
	trmt.TrmBuffSize 	= (U16)BuffSize;
}
/******************************************************************************/
/*
 * Purpose: Transmit Buffer
 * Inputs:  none
 * Output:  S_OK - All bytes transmitted, S_FALSE - Busy
 */
/******************************************************************************/
static HRESULT Transmit_Sirem_Buffer(void)
{
	U8 u8Temp;
	
	if(trmt.TrmBuffSize)	// if buffer contain data
	{
		u8Temp = *trmt.pTrmBuff;	// extract transmit byte
			
		if(IF_UART_PutByte(u8Temp) == 1)		// if single byte is transmitted
		{
			trmt.TrmBuffSize--;					// decrement buffer size with number of transmitted bytes
			trmt.pTrmBuff++;					// increment pointer of transmit buffer
		}
	}
	else
	{
		return(S_OK);
	}
	
	return(S_FALSE);
}

