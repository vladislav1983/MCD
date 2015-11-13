#include <string.h>
#include <stdio.h>
/***************************************************************************\
   	PID Function
		
	
	The PID (Proportional Integral Derivative) function is used in mainly
	control applications. PIDCalc performs one iteration of the PID
	algorithm.

	While the PID function works, main is just a dummy program showing
	a typical usage.
\***************************************************************************/

typedef struct PID	{
		double	SetPoint;         //	Desired Value

		double	Proportion;			//	Proportional Const
		double	Integral;			//	Integral Const
		double	Derivative;			//	Derivative Const

		double	LastError;			//	Error[-1]
		double	PrevError;			//	Error[-2]
		double	SumError;			//	Sums of Errors
		
}	PID;

/*=========================================================================*\
   Perform One PID Iteration
\*=========================================================================*/

double
PIDCalc			(  PID				*pp,
						double			NextPoint
         		)

{	double	dError,
				Error;

		pp->SumError += (Error = pp->SetPoint - NextPoint);
		dError = pp->LastError - pp->PrevError;
		pp->PrevError = pp->LastError;
		pp->LastError = Error;
		return   (  pp->Proportion * Error
               +  pp->Integral * pp->SumError
               +  pp->Derivative * dError
               );
}

/*=========================================================================*\
   Initialize PID Structure
\*=========================================================================*/

void
PIDInit  (  PID      *pp
         )

{     memset ( pp,0,sizeof(PID) );
}

/***************************************************************************\
	Main Program
\***************************************************************************/

double
sensor			(	void								//	Dummy Sensor Function
					)

{		return 100.0;
}

void
actuator			(	double			rDelta		//	Dummy Actuator Function
					)

{}

void
main				(	void
					)

{	PID			sPID;							//	PID Control Structure
	double		rOut;							//	PID Response (Output)
	double		rIn;							//	PID Feedback (Input)

		PIDInit ( &sPID );					//	Initialize Structure
		sPID.Proportion	= 0.5;			//	Set PID Coefficients
		sPID.Integral		= 0.5;
		sPID.Derivative	= 0.0;
		sPID.SetPoint     = 100.0;			//	Set PID Setpoint

		for (;;) {								//	Mock Up of PID Processing
			rIn = sensor ();						//	Read Input
			rOut = PIDCalc ( &sPID,rIn );		//	Perform PID Interation
			actuator ( rOut );					//	Effect Needed Changes
		}
}

