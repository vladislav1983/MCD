#ifndef TRACE_H
#define TRACE_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "basedef.h"

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define TRACE_STEPS 		200
#define NB_TRACE_VARS		6
#define TRACE_LEN 			(TRACE_STEPS + 1)

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/
struct parlist_trace
   {
   S16 *par_ptr[NB_TRACE_VARS];
   U16  trace_length;
   
   /* PARAMS STORED IN EEPROM */
   S16  trace_display[NB_TRACE_VARS];
   S16 	compare_val;
   };

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
#define _manual_start_trace()			(_getbit(trace_control,0))     	/* bit 0 */
#define _set_manual_start_trace(x)		(_putbit((x),trace_control,0))  /* bit 0 */

#define _reset_trace_manual()			(_getbit(trace_control,1))     	/* bit 1 */
#define _set_reset_trace_manual(x)		(_putbit((x),trace_control,1)) 	/* bit 1 */

/* TRACE CONTROL	
	trace configuration - full access from PC 
	Bit 0		-	MANUAL START		1: Manual Start Trace	0: Dont Start manual	
	Bit 1		-	RESET TRACE			1: Reset		
*/

#define _condition_start_bit()			(_getbit(trace_config,2))     	/* bit 2 */
#define _set_condition_start_bit(x)		(_putbit((x),trace_config,2)) 	/* bit 2 */

#define _more_than_bit()				(_getbit(trace_config,3))     	/* bit 3 */
#define _less_than_bit()				(_getbit(trace_config,4))     	/* bit 4 */
#define _equal_bit()					(_getbit(trace_config,5))     	/* bit 5 */
#define _not_equal_bit()				(_getbit(trace_config,6))     	/* bit 6 */


/* TRACE CONFIG	
	trace configuration - full access from PC 
	Bit 0		-		
	Bit 1		-		
	Bit 2		-		Condition define	1: Defined 				0: Not defined
	Bit 3		-		Condition bit 1 	1: > 					0: No Condition
	Bit 4		-		Condition bit 2 	1: < 					0: No Condition
	Bit 5		-		Condition bit 3 	1: ==					0: No Condition
	Bit 6		-		Condition bit 4 	1: != 					0: No Condition
	Bit 7		-		
*/


#define _trace_state()					(_getbit(trace_state,0))     	/* bit 0 */
#define _set_trace_state(x)				(_putbit((x),trace_state,0))    /* bit 0 */

#define _reset_trace()					(_getbit(trace_state,1))     	/* bit 1 */
#define _set_reset_trace(x)				(_putbit((x),trace_state,1))    /* bit 1 */

#define _trace_running()				(_getbit(trace_state,2))     	/* bit 2 */
#define _set_trace_running(x)			(_putbit((x),trace_state,2))    /* bit 2 */



/* TRACE STATE	
	trace state - read only from PC
	Bit 0		-		TRACE STATE 		1: READY			0: Blocked 
	Bit 1		-		RESET TRACE			1: RESET trace array	0: Don't 
	Bit 2		-		Trace Running		1: Running
*/

/*----------------------------------------------------------------------------*/
/* Exported functions                     									  */
/*----------------------------------------------------------------------------*/
U16 IF_Get_TRSB_CntBytes(void);
void trace_init_first(void);
void init_trace(void);
HRESULT Clear_Trace_Array(void);
void do_trace(void);


#endif /* TRACE_H */
