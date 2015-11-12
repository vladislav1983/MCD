/******************************************************************************/
/* 
 *                     			UART FUNCTIONS        
 *       
*/
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Body Identification                                                        */
/*----------------------------------------------------------------------------*/
#ifdef VUART_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define VUART_C
#endif

/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "includes.h"
#include "PWM.H"

/*----------------------------------------------------------------------------*/
/* Local constants                                                            */
/*----------------------------------------------------------------------------*/
#define sci_TIMER_TICKS_us				15000	// 15ms 
#define sci_TIMER_TICKS_IN_PWM_TICKS	(sci_TIMER_TICKS_us / PWM_PERIOD_us)

/* Hardware setup. */
#define serUART_ENABLED					1		//
#define serCONTINUEINIDLEMODE			1		//Discontinue operation when device enters Idle mode
#define serUSE_ALT_IO					1		//use alt io
#define serWAKE_FROM_SLEEP				0		//disable wake from sleep
#define serLOOPBACK						0		//disable loopback
#define serAUTO_BAUD					0		//disable autobaud
#define serPARITY_AND_DATA				(0b01)	//01 = 8-bit data, even parity
#define serTWO_STOP_BIT					0		//one stop bit

#define serTRANSMISSION_INTERRUPT_MODE	1		//1 = Interrupt when a character is transferred to the Transmit Shift register and as result, the transmit buffer becomes empty
#define serTRANSMIT_BREAKE_BIT			0		//0 = UxTX pin operates normally
#define serTRANSMIT_ENABLE_BIT			1		//1 = UART transmitter enabled, UxTX pin controlled by UART (if UARTEN = 1)
#define serRECIEVE_INTERRUPT_MODE		(0b00)	//0x =Interrupt flag bit is set when a character is received
#define serADDR_DETECT					0

/*----------------------------------------------------------------------------*/
/* Local macros                                                               */
/*----------------------------------------------------------------------------*/
#define _sci_timer_start(ticks) 		sci_ticks_T1 = (ticks)
#define _sci_timer_clear()				sci_tick_counter_T1 = 0										

/*----------------------------------------------------------------------------*/
/* Local types                                                                */
/*----------------------------------------------------------------------------*/
typedef struct sMessage
{
    U8   au8Buff[cUart_RcvBuffSize] __attribute__ ((aligned)); 
    U8   u8DataCnt; 					// Available data in the buffer.number of bytes in buffer.
    bool bReady;    					// Message was detected.
}tsRxMessage;



/*----------------------------------------------------------------------------*/
/* Local data                                                                 */
/*----------------------------------------------------------------------------*/
static volatile tsRxMessage gRxMsg; // Receiver object. Fill from IRQ.  Emptied from Appl.
                                    // NOTE! - gRxMsg IS NOT fully protected during access.
static volatile U16 sci_ticks_T1;
static volatile U16 sci_tick_counter_T1;

/*----------------------------------------------------------------------------*/
/* Constant local data                                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Local function prototypes                                                  */
/*----------------------------------------------------------------------------*/
void __attribute__((__interrupt__,no_auto_psv)) _U1RXInterrupt();


/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                   I N T E R F A C E   F U N C T I O N S                    */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/*
 * Purpose:   UART Module init
 */
/******************************************************************************/
void IF_Uart_Init(U32 u32Baudrate)
{
    /************************************************************************/
    /* Initialize Receive buffer */
    /************************************************************************/
    gRxMsg.u8DataCnt  = 0;
    gRxMsg.bReady  = false;

       	/* Setup the UART. */
       	
	U1MODEbits.UARTEN	= serUART_ENABLED;
	U1MODEbits.USIDL 	= serCONTINUEINIDLEMODE;
	U1MODEbits.ALTIO	= serUSE_ALT_IO;
	U1MODEbits.WAKE		= serWAKE_FROM_SLEEP;
	U1MODEbits.LPBACK	= serLOOPBACK;
	U1MODEbits.ABAUD	= serAUTO_BAUD;
	U1MODEbits.PDSEL	= serPARITY_AND_DATA;
	U1MODEbits.STSEL	= serTWO_STOP_BIT;
                         
    U1BRG = (U16)(((f32)cFCY_HZ / ((f32)16u * (f32)u32Baudrate)) - (f32)0.5); // U1BRG = Fcy/16/baudrate - 1;
    
    U1STAbits.UTXISEL	= serTRANSMISSION_INTERRUPT_MODE;
    U1STAbits.UTXBRK	= serTRANSMIT_BREAKE_BIT;
    U1STAbits.UTXEN		= serTRANSMIT_ENABLE_BIT;
    U1STAbits.URXISEL	= serRECIEVE_INTERRUPT_MODE;
    U1STAbits.ADDEN		= serADDR_DETECT;
  
    _U1RXIP = cISR_PRIORITY_U1RX;           // set priority level
    _U1TXIP = cISR_PRIORITY_U1TX;
    _U1RXIF = 0;                            // clear interrupt flags
    _U1TXIF = 0;
    _U1RXIE = 1;                            // Rx Interrupt Enable
    _U1TXIE = 0;                            // Tx Interrupt Disable
	
	_sci_timer_start(sci_TIMER_TICKS_IN_PWM_TICKS); //<------ INIT Here sci timer
}

/******************************************************************************/
/*
 * Name:      Check if message is recieved
 * Purpose:   0- no message, or number of availible bytes
 */
/******************************************************************************/
U8 IF_Uart_MsgGetDataCnt(void)
{
    bool bRdy;

    bRdy = gRxMsg.bReady;     // critical section manipulation

    if( bRdy )	//if ready
    {
        return(gRxMsg.u8DataCnt);	//return number of recieved bytes
    }
    else return 0; // no available message
    
}
/******************************************************************************/
/*
 * Purpose:   Reads characters from the INPUT(Rx).
 * Inputs:
 *  - Destination buffer to store read characters.
 *  - Size of buffer
 * Output: Number of characters actually read.
 */
/******************************************************************************/
HRESULT IF_Uart_MsgGet(U8 *pDestinBuff)
{
    U8 u8CntBytes;

    if( gRxMsg.bReady )		//if message ready
    {
        u8CntBytes = gRxMsg.u8DataCnt;	//

        while( 0<u8CntBytes )	//while all bytes
        {
            u8CntBytes--;
            pDestinBuff[u8CntBytes] = gRxMsg.au8Buff[u8CntBytes];//destination buff filled with number of bytes u8CntBytes
        }

        gRxMsg.u8DataCnt = 0;	//all bytes transferred
        gRxMsg.bReady    = false;    // there no anymore valid frame.
        return S_OK;	//return OK.
    }
    else
    {
        // no available message
        return S_FALSE;	//return no message
    }
}
/******************************************************************************/
/*
* Purpose: Purge input Rx buffer
* Output:  ---
*/
/******************************************************************************/
void IF_Uart_RxPurge(void)
{
     if( gRxMsg.bReady )	//if availible message
    {
        gRxMsg.u8DataCnt = 0;			//no bytes
        gRxMsg.bReady    = false;    	// no more message
    }
    
}
/******************************************************************************/
/*
 * Purpose: 
 * Inputs:  pSrcBuff   - Source buffer for transmitting
 *          u8CntBytes - Size of buffer
 * Output:  Actually written bytes count.
 *   
 */
/******************************************************************************/
U16 IF_Uart_MsgPut(U8 *pSrcBuff, U16 u16CntBytes)
{
    U16 u16Index;               // Input buffer index.


    _U1RXIE = 0;                // Rx Interrupt Disable. Echo are received here.

    u16Index = 0;
    while( u16Index<u16CntBytes )
    {
        U1TXREG = pSrcBuff[u16Index];            // transfer to serial shift reg.
		
         //wait byte transmission finish. used FIFO functionalities.
        while(U1STAbits.UTXBF) {};
        
        u16Index++;
    }
    
    _U1RXIF = 0;            // clear interrupt flag
    _U1RXIE = 1;            // Rx Interrupt Enable

    return u16Index;        // actually written bytes
}
/******************************************************************************/
/*
 * Purpose: Send a single byte via UART
 * Inputs:  U8 byte to transmit
 *          
 * Output:  Transmitted bytes
 *        
 */
/******************************************************************************/
U8 IF_UART_PutByte(U8 u8TByte)
{
	U8 tx_bytes = 0;
	
	_U1RXIE = 0;                // Rx Interrupt Disable. Echo are received here.
	
	if(U1STAbits.UTXBF == 0) // If Transmit Buffer is not full
	{
		U1TXREG = u8TByte;
    	tx_bytes++;
 	}   
    else // transmit buffer full
	{
    	tx_bytes = 0;
    }
    
    _U1RXIF = 0;            // clear interrupt flag
    _U1RXIE = 1;            // Rx Interrupt Enable

	return(tx_bytes);
}
/******************************************************************************/
/*
 * Name:      UART Interrupt handler
 * Purpose:   Stores a byte in a Rx buffer when it is received on Uart.
 */
/******************************************************************************/
void __attribute__((__interrupt__,no_auto_psv)) _U1RXInterrupt()
{
    U8 u8Temp;
     
     _U1RXIF = 0;       // clear interrupt flag
     
	_sci_timer_clear(); //<------ Clear Here sci handle timer
	
	//sci_watchdog_timer(1, sci_WDT_TIMER_TIMEOUT_RX);
	
    while( U1STAbits.PERR || U1STAbits.FERR )   //while no error
    {
        u8Temp = (U8)U1RXREG; //read RX reg
    }

	
    while((U1STAbits.URXDA) && (gRxMsg.u8DataCnt<cUart_RcvBuffSize)) //while U1STAbits.URXDA=1 && (gRxMsg.u8DataCnt<cUart_RcvBuffSize=10!)
    {
     // Read from FIFO until it has a byte, and circular buffer is NOT full.
     gRxMsg.au8Buff[gRxMsg.u8DataCnt] = (U8)U1RXREG;
     gRxMsg.u8DataCnt++;
    }
	
	U1STAbits.OERR=0;  // clear overrun error flag
    
}

/******************************************************************************/
/*
 * Purpose: Serial Communication Timer. If 15ms time is expre apply valid sirem data. Run in every TASK1 end.
 * Inputs:  none
 * Output:	none
 * 
 */
/******************************************************************************/
inline void sci_timer(void)
{
	sci_tick_counter_T1++;
	
	if(sci_tick_counter_T1 == sci_ticks_T1)
	{
		_sci_timer_clear();
	
    	if(0!=gRxMsg.u8DataCnt)
    	{
    	    gRxMsg.bReady = true;
    	}
 	}   
}


