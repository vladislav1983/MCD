#ifndef DIGITALIO_H
#define DIGITALIO_H
    
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
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
#define DRIVE_ENABLE()			pinDRV_EN = 1;	\
								_set_drive_enabled(1);

#define DRIVE_DISABLE()			pinDRV_EN = 0;	\
								_set_drive_enabled(0);

#define RESET_DIO_TEST_REG()	DIO_TEST_REG = 0



extern U16 DIO_TEST_REG;

/*	
	DIO_TEST_REG 
	
	Digital Outputs
	
	Bit 0		-	RF5 - LED1
	Bit 1		-	RF4 - LED2
	Bit 2		-	RF1 - LED3
	Bit 3		-	RF0 - LED4
	Bit 4		-	RD0 - Fault Reset
	Bit 5		-	RD1 - PWM Enable
	
	Digital Inputs
	
	Bit 6		-	RG6 - Button 1
	Bit 7		-	RG7 - Button 2
	Bit 8		-	RG8 - Button 3
	Bit 9		-	RG9 - Button 4
	Bit 10		-	RD8 - Fault Latch
	Bit 11		-
	Bit 12		-
	Bit 13		-
	Bit 14		-	
	Bit 15		-	
*/

/* Digital Outputs */
#define _PC_PORTF_5()						(_getbit(DIO_TEST_REG,0))  	  	/* bit 0 */
#define _set_PC_PORTF_5(x)					(_putbit((x),DIO_TEST_REG,0)) 	/* bit 0 */

#define _PC_PORTF_4()						(_getbit(DIO_TEST_REG,1))  	  	/* bit 1 */
#define _set_PC_PORTF_4(x)					(_putbit((x),DIO_TEST_REG,1)) 	/* bit 1 */

#define _PC_PORTF_1()						(_getbit(DIO_TEST_REG,2))  	  	/* bit 2 */
#define _set_PC_PORTF_1(x)					(_putbit((x),DIO_TEST_REG,2)) 	/* bit 2 */

#define _PC_PORTF_0()						(_getbit(DIO_TEST_REG,3))  	  	/* bit 3 */
#define _set_PC_PORTF_0(x)					(_putbit((x),DIO_TEST_REG,3)) 	/* bit 3 */

#define _PC_PORTD_0()						(_getbit(DIO_TEST_REG,4))  	  	/* bit 4 */
#define _set_PC_PORTD_0(x)					(_putbit((x),DIO_TEST_REG,4)) 	/* bit 4 */

#define _PC_PORTD_1()						(_getbit(DIO_TEST_REG,5))  	  	/* bit 5 */
#define _set_PC_PORTD_1(x)					(_putbit((x),DIO_TEST_REG,5)) 	/* bit 5 */

/* Digital Inputs */
#define _PC_PORTG_6()						(_getbit(DIO_TEST_REG,6))  	  	/* bit 6 */
#define _set_PC_PORTG_6(x)					(_putbit((x),DIO_TEST_REG,6)) 	/* bit 6 */

#define _PC_PORTG_7()						(_getbit(DIO_TEST_REG,7))  	  	/* bit 7 */
#define _set_PC_PORTG_7(x)					(_putbit((x),DIO_TEST_REG,7)) 	/* bit 7 */

#define _PC_PORTG_8()						(_getbit(DIO_TEST_REG,8))  	  	/* bit 8 */
#define _set_PC_PORTG_8(x)					(_putbit((x),DIO_TEST_REG,8)) 	/* bit 8 */

#define _PC_PORTG_9()						(_getbit(DIO_TEST_REG,9))  	  	/* bit 9 */
#define _set_PC_PORTG_9(x)					(_putbit((x),DIO_TEST_REG,9)) 	/* bit 9 */

#define _PC_PORTD_8()						(_getbit(DIO_TEST_REG,10))  	/* bit 10 */
#define _set_PC_PORTD_8(x)					(_putbit((x),DIO_TEST_REG,10)) 	/* bit 10 */


/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void IF_DigitalIO_Init(void);
void PU_Fault_Reset(void);
void User_LEDS_Control(void);
void DIO_Test(void);


#endif /* DIGITALIO_H */
