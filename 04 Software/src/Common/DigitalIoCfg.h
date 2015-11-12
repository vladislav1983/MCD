/******************************************************************************/
/* 						DIGITAL IO DEFINITION
 *                             
 *       
 */
/******************************************************************************/
#ifndef DIGITALIOCFG_h
#define DIGITALIOCFG_h
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/
#include "basedef.h"

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                              */
/*----------------------------------------------------------------------------*/

/* USER LED Pins */
#define pinERROR_LED			_LATF5
#define pinERROR_LED_dir		_TRISF5

#define pinSTATUS_LED			_LATF4
#define pinSTATUS_LED_dir		_TRISF4

#define pinPUF_LED				_LATF1
#define pinPUF_LED_dir			_TRISF1

#define pinSTATE_LED			_LATF0
#define pinSTATE_LED_dir		_TRISF0

/* User Buttons Pins */
#define pinBUT1					_RG6
#define pinBUT1_dir				_TRISG6

#define pinBUT2					_RG7
#define pinBUT2_dir				_TRISG7

#define pinBUT3					_RG8
#define pinBUT3_dir				_TRISG8

#define pinBUT4					_RG9
#define pinBUT4_dir				_TRISG9

/* PU Fault Pins */
#define pinPU_FAULT				_RD8
#define pinPU_FAULT_dir			_TRISD8


/* Powe Unit Control Pins */
#define pinPU_FAULT_RST			_LATD0		// Fault Reset 
#define pinPU_FAULT_RST_dir		_TRISD0

#define pinDRV_EN				_LATD1		// Drive Enable
#define pinDRV_EN_dir			_TRISD1
/*----------------------------------------------------------------------------*/
/* Pins declaration                                    		                  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/

#endif /* DIGITALIOCFG_h */
