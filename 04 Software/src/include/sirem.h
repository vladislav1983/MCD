#ifndef SIREM_H
#define SIREM_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data     SIREM FRAMES                                                         */
/*----------------------------------------------------------------------------*/
#define cSiremIndex_FrameStart  0        // Field index into sirem frame.
#define cSiremIndex_Cmd         1
#define cSiremIndex_Chks        9

#define cSiremFrameSize         10       // Sirem frame count bytes

#define cSiremCmd_ReadPrmInfo   0x78     // Field Command values
#define cSiremCmd_ReadPrmList   0x7E
#define cSiremCmd_ReadPrm       0x68
#define cSiremCmd_WritePrm      0x91
#define cSiremCmd_ReadTrace     0x71
#define cSiremCmd_Restart       0x57

#define cSiremValueLowFirst     0x00     // value for field Endianes (ReadPrmInfo)
#define cSiremValueHighFirst    0xFF

#define cSiremValueFrameStart   0xA9     // value for field FrameStart

#define cSoftwareVersion        ((U32)0x12345678ul)

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
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
void Sirem_Engine(void);


#endif /* SIREM_H */
