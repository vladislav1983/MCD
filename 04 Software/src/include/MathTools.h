#ifndef MATHTOOLS_H 
#define MATHTOOLS_H
    
/*----------------------------------------------------------------------------*/
/* Included files to resolve specific definitions in this file                */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Constant data                                                              */
/*----------------------------------------------------------------------------*/
#define 	SINTAB_LEN			512 	// 2^9
#define		SIN_LEN_DIV			9

#define 	SQRT_TAB_LEN		512 	// 2^9
#define 	SQRT_LEN_DIV		9

#define 	ATAN_TABLE_LEN		129
#define 	DEC_PLACES_BEFORE 	7     	/* Corresponds to 128 values */
#define 	DEC_PLACES_AFTER 	(15-DEC_PLACES_BEFORE)

#define 	_SDIV				1
#define 	_UDIV				0

/*
*	arithmetic constants
*	====================
*/
#define PI				3.141592654
#define SQRT2			1.414213562
#define SQRT3			1.732050808
#define SQRT3OV2		0.866025403
/*
*	Fractional arithmetic constants
*	====================
*/
#define qOneBySq3		0x49E7			// 1/SQRT(3)*32768 (Fractional format)
#define qOneBy3			0x2AAB			// 1/3 * 32768 in fractional format
#define qSQRT3OV2		0x6EDA			// SQRT(3)/2 in Fractional Format
#define qSQRT2			0xB504			// SQRT(2)   in Fractional Format
#define qSQRT3			0xDDB3			// SQRT(3)   in Fractional Format
#define qPI_deg			32768
#define qHalf_PI_deg	(qPI_deg / 2)
/*----------------------------------------------------------------------------*/
/* Exported type                                                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported data                                                             */
/*----------------------------------------------------------------------------*/
extern const S16	sintab[];

/*----------------------------------------------------------------------------*/
/* Constant exported data                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Exported Macros                                                            */
/*----------------------------------------------------------------------------*/
/******************************************************************************/
/*
* Purpose:  Sine Function with linear interpolation in a small degrees input.
* Input: 	alpha = 2^16*x/(2*PI)
* Output:   sin(x)*32768
*/
/******************************************************************************/
static inline S16 sine(U16 alpha)	
{	
if (  ( ((S16)alpha) <  (S16)( 0.25*65536.0/(2*PI)) ) && ( ((S16)alpha) >  (S16)(-0.25*65536.0/(2*PI)) )  )
   return((S32)(256*PI)*(S16)alpha >> 8);
else
   return(sintab[(U16)(alpha)>>(16-SIN_LEN_DIV)]);
}

/******************************************************************************/
/*
* Purpose:  Cosine Function with linear interpolation in a small degrees input.
* Input: 	alpha = 2^16*x/(2*PI)
* Output:   cos(x)*32768
*/
/******************************************************************************/
#define cosine(alpha)	sine(16384 - (alpha))

/*
*	macro definitions for arithmetic functions
*	==========================================
*/
#define absi(x)             ((x)>=0 ? (x) : -(x))		/* absolut of integer */
#define sign(x)			    ((x)>=0 ? 1 : -1)			/* sign function 1/-1 */
#define sigm(x)			    ((x)>=0 ? 0 : 1)			/* sign function 0/1 */
#define signmul(x,y)		((y)>=0 ? (x) : -(x))		/* set sign(x) according sign(y) */
#define max(x,y)			((x)>=(y) ? (x) : (y))		/* return maximum of (x,y) */
#define min(x,y)			((x)>=(y) ? (y) : (x))		/* return minimum of (x,y) */
#define boolean(x)	        ((x)==0 ? FALSE : TRUE)		/* integer to boolean conversion */
#define MulFracBy2(x,y)     ((((S32)(x)*(y))<<1)>>16)   /* operands word */
#define MulFracBy2L(x,y)    (((x)*(y))>>15)             /* operands long */
#define div_mod2(x,n)       (((x)+(1<<((n)-1)))>>(n))   /* division modulo 2 with rounding, n >= 1! */
#define HiPartBy2(x)        ((x)>>15)

#define Hi(LongVar)		    (*((S16 *)&(LongVar)+1))
#define Lo(LongVar)         (*(S16 *)&(LongVar))

#define Hiu(LongVar)	    (*((U16 *)&(LongVar)+1))
#define Lou(LongVar)        (*(U16 *)&(LongVar))

#define HiBy(WordVar)	    (*((U8 *)&(WordVar)+1))
#define LoBy(WordVar)       (*(U8 *)&(WordVar))


/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/
S16 sqrt_16(U16 rad);
S16 sqrt_32(U32 rad);
U32 pow_16(S16 x, U16 n);
void DSP_Engine_Init(void);
U16 ATAN(S16 iY, S16 iX);
S16 _div_32(S32 sDivident_long, S16 sDivider, bool Div_Type);
S16 _fmul(S16 qA, S16 qB);
S32 _mul_16(S16 A, S16 B);


#endif /* MATHTOOLS_H */
