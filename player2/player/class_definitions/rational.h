/************************************************************************
COPYRIGHT (C) SGS-THOMSON Microelectronics 2007

Source file name : rational.h
Author :           Nick

Definition of the class supporting rationals in player 2.


Date        Modification                                    Name
----        ------------                                    --------
20-Apr-07   Created                                         Nick

************************************************************************/


#ifndef H_RATIONAL
#define H_RATIONAL

#define 	Abs(v)	(((v)<0) ? -(v) : (v))

// -------------------------------------------------------------

class Rational_c
{
private:
	bool			Negative;
	unsigned long long	Numerator;
	unsigned long long	Denominator;

public:

    Rational_c( long long	N = 0,
		long long	D = 1 )
    {
	Negative	= ((N < 0) ^ (D < 0));
	Numerator	= Abs(N);
	Denominator	= Abs(D);
    }

//

    Rational_c( unsigned long long	N,
		unsigned long long	D,
		unsigned long long 	R,
		bool 			Neg )
    {
	Negative	= Neg;
	Numerator	= N;
	Denominator	= D;

	//
	// This is a cheat, whenever we do a calculation, we often 
	// have a value that may divide both top and bottom.
	// Here we check it (note I provided three as a default possible.
	//

	if( R &&
	    (R*(Numerator/R) == Numerator) &&
	    (R*(Denominator/R) == Denominator) )
	{
	    Numerator	= Numerator/R;
	    Denominator	= Denominator/R;
	}
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Restriction function, brings the numerator and 
    // denominator down to 32 bit values.
    //

    void   Restrict(	void )
    {
	unsigned long long 	Tmp;
	unsigned int 		Shift		= 0;
	unsigned int		SpareBitsForArithmetic;
	unsigned long long	NewDenominator;

//

	Tmp	= (Numerator | Denominator) >> 32;

	if( Tmp != 0 )
	{
	    Shift	+= ((Tmp & 0xffff0000) != 0) ? 16 : 0;
	    Shift	+= (((Tmp >> Shift) & 0x0000ff00) != 0) ? 8 : 0;
	    Shift	+= (((Tmp >> Shift) & 0x000000f0) != 0) ? 4 : 0;
	    Shift	+= (((Tmp >> Shift) & 0x0000000c) != 0) ? 2 : 0;
	    Shift	+= (((Tmp >> Shift) & 0x00000002) != 0) ? 1 : 0;

	    //
	    // We know how much we are going to shift, however we have a problem
	    // if the denominator is small and the numerator is large, we may well
	    // seriously damage the accuracy, so, if it is possible we scale both 
	    // numerator and the denominator so that the shift will not hurt the 
	    // accuracy (I know this sounds highly dubious, but it appears to work
	    // so there).
	    //

	    SpareBitsForArithmetic	= 31 - Shift; 		// Careful we do not overflow doing this scale, 
	    if( Denominator < (1ULL << SpareBitsForArithmetic) )
	    {
		NewDenominator	= Denominator & (0xffffffff << (Shift + 1));
		Numerator	= (Numerator * NewDenominator) / Denominator;
		Denominator	= NewDenominator;
	    }

//

	    Numerator		= (Numerator + (1 << Shift)) >> (Shift + 1);
	    Denominator		= (Denominator + (1 << Shift)) >> (Shift + 1);
	}

	//
	// Empirically I noticed three oft repeated conditions
	// which cause me to shift, and lose some accuracy.
	// The first two are when one of the denominator, or
	// numerator is a multiple of the other, the third 
	// involves the use of factors of 10, I have elliminated 
	// both cases, and reduced the accuracy losing shift a 
	// great deal.
	//

	if( (Denominator != 1) && (Numerator != 1) && (Denominator != 0) )
	{
	    if( (Denominator * (Numerator/Denominator)) == Numerator )
	    {
	    	Numerator		= Numerator/Denominator;
	    	Denominator		= 1;
	    }
	    else if( (Numerator * (Denominator/Numerator)) == Denominator )
	    {
	    	Denominator		= Denominator/Numerator;
	    	Numerator		= Numerator/Numerator;
	    }
	    else while( ((10*(Numerator/10)) == Numerator) && 
			((10*(Denominator/10)) == Denominator ) )
	    {
		Numerator		= Numerator/10;
		Denominator		= Denominator/10;
	    }
	}
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Truncate function - restrict the denominator and truncate
    //

    Rational_c	TruncateDenominator( unsigned int	NewDenominator )
    {
	Rational_c	Result( (NewDenominator * Numerator)/Denominator, NewDenominator, 3, Negative );
	return Result;
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Assignment functions
    //

    bool   operator= (	int 	V )
    {
	Negative	= V < 0;
    	Numerator	= Abs(V);
	Denominator	= 1;
	return true;
    }

//

    bool   operator= (	Rational_c	R )
    {
	Negative	= R.Negative;
    	Numerator	= R.Numerator;
	Denominator	= R.Denominator;
	Restrict();
	return true;
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Comparison
    //

    bool   operator== (		Rational_c	F )
    {
	return ((Negative == F.Negative) && ((Numerator * F.Denominator) == (F.Numerator * Denominator)));
    }

//

    bool   operator== (		int	I )
    {
	return (Negative == (I < 0)) && (Numerator == (Abs(I) * Denominator));
    }

//

    bool   operator!= (		Rational_c	F )
    {
	return ((Negative != F.Negative) || ((Numerator * F.Denominator) != (F.Numerator * Denominator)));
    }

//

    bool   operator!= (		int	I )
    {
	return (Negative != (I < 0)) || (Numerator != (Abs(I) * Denominator));
    }

//

    bool   operator>= (		Rational_c	F )
    {
	return	Negative ?
			(F.Negative ?
				((Numerator * F.Denominator) <= (F.Numerator * Denominator)) :
				((Numerator == 0) && (F.Numerator == 0)) ) :
			(F.Negative ?
				(true) :
				(Numerator * F.Denominator) >= (F.Numerator * Denominator));
    }

//

    bool   operator>= (		int	I )
    {
	Rational_c	Temp = I;
	return *this >= Temp;
    }

//

    bool   operator> (		Rational_c	F )
    {
	return	Negative ?
			(F.Negative ?
				((Numerator * F.Denominator) < (F.Numerator * Denominator)) :
				(false) ) :
			(F.Negative ?
				((Numerator == 0) && (F.Numerator == 0)) :
				(Numerator * F.Denominator) > (F.Numerator * Denominator));
    }

//

    bool   operator> (		int	I )
    {
	Rational_c	Temp = I;
	return *this > Temp;
    }

//

    bool   operator<= (		Rational_c	F )
    {
	return !(*this > F);
    }

//

    bool   operator<= (		int	I )
    {
	return !(*this > I);
    }

//

    bool   operator< (		Rational_c	F )
    {
	return !(*this >= F);
    }

//

    bool   operator< (		int	I )
    {
	return !(*this >= I);
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Add and Subtract
    //

    Rational_c   operator+ (	Rational_c	F )
    {
	if( Negative ^ F.Negative )
	{
	    return *this - Rational_c(F.Numerator, F.Denominator, 3, !F.Negative);
	}
	else
	{
	    Rational_c	Result	= Rational_c(  (Numerator * F.Denominator) + (F.Numerator * Denominator),
						Denominator * F.Denominator,
						Denominator,
						Negative );
	    return Result;
	}
    }

//

    Rational_c   operator+ (	int	I )
    {
	Rational_c	Temp = I;
	return *this + Temp;
    }

//

    Rational_c   operator+= (	Rational_c	F )
    {
    	return (*this = *this + F);
    }

//

    Rational_c   operator+= (	int	I )
    {
	return (*this = *this + I);
    }
    
//

    Rational_c   operator- (	Rational_c	F )
    {
	if( Negative ^ F.Negative )
	{
	    return *this + Rational_c(F.Numerator, F.Denominator, 3, !F.Negative);
	}
	else
	{
	    bool	A_GT_B	= (Numerator * F.Denominator) > (F.Numerator * Denominator);
	    Rational_c  Result;

	    if( A_GT_B )
	    {
		Result		= Rational_c(	(Numerator * F.Denominator) - (F.Numerator * Denominator),
                                                Denominator * F.Denominator,
                                                Denominator,
						Negative );
	    }
	    else
	    {
		Result		= Rational_c(	(F.Numerator * Denominator) - (Numerator * F.Denominator),
                                                Denominator * F.Denominator,
                                                Denominator,
						!Negative );
	    }
	    return Result;
	}

    }

//

    Rational_c   operator- (	int	I )
    {
	Rational_c	Temp = I;
	return *this - Temp;
    }

//
    Rational_c   operator-= (	Rational_c	F )
    {
	return (*this = *this - F);
    }

//

    Rational_c   operator-= (	int	I )
    {
	return (*this = *this - I);
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Multiply
    //

    Rational_c   operator* (	Rational_c	F )
    {
	Rational_c	Product( Numerator * F.Numerator, Denominator * F.Denominator, min(Denominator,F.Denominator), (Negative ^ F.Negative) );
	return Product;
    }

//

    Rational_c   operator* (	int	V )
    {
	Rational_c	Product( Numerator * Abs(V), Denominator, min(Denominator,Abs(V)), (V < 0) ^ Negative );
	return Product;
    }

    Rational_c   operator*= (	Rational_c	F )
    {
	return (*this = *this * F);
    }

//

    Rational_c   operator*= (	int	V )
    {
	return (*this = *this * V);
    }


    // ////////////////////////////////////////////////////////////////////////
    //
    // Divide
    //

    Rational_c   operator/ (	Rational_c	F )
    {
	long long Factor	= 10;

	if( Numerator == F.Numerator )
	    Factor	= Numerator;
	else if( Denominator == F.Denominator )
	    Factor	= Denominator;

	Rational_c	Result( Numerator * F.Denominator, Denominator * F.Numerator, Factor, (Negative ^ F.Negative) );
	return Result;
    }

//

    Rational_c   operator/ (	int	V )
    {
	Rational_c	Result( Numerator, Denominator * Abs(V), Abs(V), (V < 0) ^ Negative );
	return Result;
    }

//

    Rational_c   operator/= (	Rational_c	F )
    {
	return (*this = *this / F);
    }

//

    Rational_c   operator/= (	int	V )
    {
	return (*this = *this / V);
    }


    // ////////////////////////////////////////////////////////////////////////
    //
    // Extraction
    //

    long long   GetNumerator( void )
    {
        return Negative ? -Numerator : Numerator;
    }

//

    long long   GetDenominator( void )
    {
        return Denominator;
    }

//

    long long   LongLongIntegerPart( void )
    {
	if (Denominator == 0) return -1ll;
	else return (Negative ? -(Numerator / Denominator) : (Numerator / Denominator));
    }

//

    int   IntegerPart(	void )
    {
	return (int)(Negative ? -(Numerator / Denominator) : (Numerator / Denominator));
    }

//

    long long   RoundedLongLongIntegerPart( void )
    {
	return (Negative > 0) ?
		    -((Numerator + (Denominator/2)) / Denominator) :
		     ((Numerator + (Denominator/2)) / Denominator);
    }

//

    int   RoundedIntegerPart(	void )
    {
	return (Negative > 0) ?
		    -(int)((Numerator + (Denominator/2)) / Denominator) :
		     (int)((Numerator + (Denominator/2)) / Denominator);
    }

//
    
    long long   RoundedUpLongLongIntegerPart( void )
    {
	return (Negative > 0) ?
		    -((Numerator + (Denominator-1)) / Denominator) :
		     ((Numerator + (Denominator-1)) / Denominator);
    }

//
    
    int   RoundedUpIntegerPart(	void )
    {
	return (Negative > 0) ?
		    -(int)((Numerator + (Denominator-1)) / Denominator) :
		     (int)((Numerator + (Denominator-1)) / Denominator);
    }
    
//
    
    Rational_c   Remainder( void )
    {
#if 0
	Rational_c	Result( Numerator % Denominator, Denominator, 2, Negative );
#else
// mod does not work in the kernel for 64 bit numbers
	Rational_c	Result( Numerator - (Denominator * ( Numerator / Denominator )), Denominator, 3, Negative );
#endif

	return Result;
    }

//

    int   RemainderDecimal( unsigned int Places = 6 )
    {
	Rational_c	Tmp	= Remainder();

	switch( Places )
	{
	    case	3:	Tmp.Numerator 	*= 1000;				break;
	    case	6:	Tmp.Numerator 	*= 1000000;				break;
	    default:
	    case	9:	Tmp.Numerator 	*= 1000000000;				break;
	}

	return Tmp.Numerator/Tmp.Denominator;
    }

    // ////////////////////////////////////////////////////////////////////////
    //
    // Report
    //

    void   Print( const char *S = "" )
    {
	report( severity_info, "%s{%d %016llx, %016llx}\n", S, Negative, Numerator, Denominator );
    }
};

typedef Rational_c Rational_t;

// //////////////////////////////////////////////////////////////////////////////////////////
//
//	The inconvenient bunch where the non rational is on the left
//

static inline Rational_c   operator+ (	int		I,
					Rational_c	F )
{
    Rational_c	Temp = I;
    return Temp + F;
}

//

static inline Rational_c   operator- (	int		I,
					Rational_c	F )
{
    Rational_c	Temp = I;
    return Temp - F;
}

//

static inline Rational_c   operator* (	int		V,
					Rational_c	F )
{
    Rational_c	Temp = V;
    return Temp * F;
}

//

static inline Rational_c   operator/ (	int		V,
					Rational_c	F )
{
    Rational_c	Temp = V;
    return Temp / F;
}


// //////////////////////////////////////////////////////////////////////////////////////////
//
//	Supplying the useful extraction bits as standalone functions
//

static inline long long   LongLongIntegerPart( Rational_c	R )
{
    return R.LongLongIntegerPart();
}

//

static inline int   IntegerPart(	Rational_c	R )
{
    return R.IntegerPart();
}

//

static inline long long   RoundedLongLongIntegerPart( Rational_c	R )
{
    return R.RoundedLongLongIntegerPart();
}

//

static inline int   RoundedIntegerPart(	Rational_c	R )
{
    return R.RoundedIntegerPart();
}

//

static inline Rational_c   Remainder( Rational_c	R )
{
    return R.Remainder();
}

//

static inline int   RemainderDecimal( Rational_c	R )
{
    return R.RemainderDecimal();
}

//

static inline Rational_c   TruncateDenominator( Rational_c	R,
						unsigned int	N  )
{
    return R.TruncateDenominator(N);
}

#endif