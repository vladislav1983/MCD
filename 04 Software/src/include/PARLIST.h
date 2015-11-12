#ifndef PARLIST_H
#define PARLIST_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/
typedef struct tIOlist
{
    U16   number 		__attribute__ ((packed));      // parameter number
    char* name 			__attribute__ ((packed));        // calling name, max. 10 char.
    void* write_adress 	__attribute__ ((packed));
    void* read_adress 	__attribute__ ((packed));
    U16   size 			__attribute__ ((packed));		// number of bytes
    U8    type[4] 		__attribute__ ((packed));		// type[0]: 's'=signed
                        						//  		'u'=unsigned
                        						//          'p'=pointer
                        						//          'f'=float
                        						//          'a'=array
                        						//          'b'=bitmap
                        						// type[1]: 'r'=RAM    (write)
                        						//          'n'=NOVRAM
                        						//          'o'=ROM
                        						// type[2]: like type[1] (read)
    S32   max 			__attribute__ ((packed));         // maximum value
    S32   min;          						// minimum value
    U16   passwd_write 	__attribute__ ((packed));
    U16   passwd_read 	__attribute__ ((packed)); //  0 = free
                        						//  1 = operator
                        						//  2 = engineer
                        						//  3 = service
                        						//  4 = programmer
                        						//  >4= no access*/
    S16   scale 		__attribute__ ((packed));       // (int)value/(float)value
}iolist;

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
U16 IF_Parlist_GetCntMembers(void);
U16 IF_Parlist_GetCntBytes(void);
iolist* IF_Parlist_bnu(U16 u16PrmNumber);
void Params_check_limit(void);
void Params_Conversion(void);

#endif /* PARLIST_H */
