#ifndef BASEDEF_H
#define BASEDEF_H

typedef unsigned char   U8;
typedef unsigned int    U16;
typedef unsigned long   U32;
typedef unsigned long long U64;

typedef signed char     S8;
typedef signed int      S16;
typedef signed long     S32;

typedef float           f32;
typedef double          f64;

typedef U8          	bool;
#define false       	((bool)(0))
#define true        	((bool)(!false))

typedef U8          	HRESULT;
#define S_OK            ((HRESULT)0x00u)
#define S_FALSE         ((HRESULT)0x01u)
#define E_INVALIDARG    ((HRESULT)0x02u)
#define E_FAIL          ((HRESULT)0x03u)

#if !defined NULL
#define NULL ((void*)0)
#endif

/* TOOLS AND MACROS */



#define _putbit(value,dst,bitn)	 (		\
{										\
	if(value)							\
	dst = ((dst) | (U16)(1u << bitn));	\
	else								\
	dst = ((dst) & (U16)~(1u << bitn));	\
})

#define _getbit(src,bitn)		((src) & (1 << (bitn)))

#define absi(x)         ((x)>=0 ? (x) : -(x))			/* absolut of integer */

#endif /* BASEDEF_H */
