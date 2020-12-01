/*
 *  ======== bsc.c ========
 *
 *  Basic Analysis Module
 *
 *  Copyright (c) 1995-1996 by QIC:Solutions Inc.  All rights reserved.
 */

#include <Windows.h>
#include "bsc.h"

/* general definitions */
#define ZCR_THRESHOLD	5
#define MAX_UP_VALUE	512
#define MAX_DOWN_VALUE	1024

/*
 *  ======== BSC_Init ========
 *
 *  Purpose:
 *	Module initialization.
 *
 *  Return value:
 *	Returns 0 if operation successful; otherwise -1 is returned.
 */

int BSC_Init( BSC_Context *context )
{
    /* initialize structure */
	RtlZeroMemory( context, sizeof( BSC_Context ) );

    /* set init flag */
	context->lastIsPos = TRUE;
	context->init = TRUE;

    return 0;
}

/*
 *  ======== BSC_Analyze ========
 *
 *  Purpose:
 *	Analyze the input buffer to determine base characterizations from
 *	the input buffer.  Scale data in anticipation of future fixed point
 *	processing.
 *
 *  Return value:
 *	Returns 0 if operation successful; otherwise -1 is returned.
 */
 
int BSC_Analyze( BSC_Context *context, short *inBuffer, short *outBuffer )
{
    short	*ptr;
    unsigned short	i, max, tabs;

    /* if module not initialized, return */
	if ( !context->init )
	{
	    return -1;
	}

    /* perform DC removal (just copy for now!) */
	for(i = 0; i < BSC_FRAME_SIZE; i++)
	{
		outBuffer[i] = inBuffer[i] >> 2;
	}

    /* find max peak, zero crossing, and mean values */
	max = 1;
	context->absSum = 0;
	context->zCross = 0;

	for ( ptr = outBuffer; ptr < ( outBuffer + BSC_FRAME_SIZE ); ptr++ )
	{
	    /* take absolute value of current sample */
		tabs = BSC_abs( *ptr );

	    /* update absolue sum */
		context->absSum += tabs;

	    /* check for new maximum */
		if ( tabs > max )
        {
            max = tabs;
        }

	    /* update zero crossings if required */
		if ( *ptr >= 0 )
		{
		    if ( ( *ptr >= ZCR_THRESHOLD ) && !context->lastIsPos )
		    {
				context->zCross++;
				context->lastIsPos = TRUE;
		    }
		}
		else
		{
		    if ( ( *ptr <= -ZCR_THRESHOLD ) && context->lastIsPos )
		    {
				context->zCross++;
				context->lastIsPos = FALSE;
		    }
		}
	}

	context->max = max;
	context->mean = ( BSC_SignalLevel ) ( context->absSum >> BSC_MEAN_SHIFT );

    /* avoid boundary condition */
#ifdef CONDENSE
	context->mean++;
#else
	if ( context->mean == 0 )
	{
	    context->mean = 1;
	}
#endif

    /* calculate scale factors */
	context->upFactor = 0;
	context->downFactor = 0;

    /* find up factor */
	while ( max < MAX_UP_VALUE )
	{
		context->upFactor++;
		max <<= 1;
	}

    /* find down factor */
    while ( max >= MAX_DOWN_VALUE )
    {
        context->downFactor++;
        max >>= 1;
    }

    /* scale data */
    if ( context->upFactor != 0 )
    {
        for ( ptr = outBuffer; ptr < ( outBuffer + BSC_FRAME_SIZE ); ptr++ )
		{
			*ptr <<= context->upFactor;
		}
    }
    else if ( context->downFactor != 0 )
    {
        for ( ptr = outBuffer; ptr < ( outBuffer + BSC_FRAME_SIZE ); ptr++ )
        {
			*ptr >>= context->downFactor;
        }
    }

    return 0;
}

/*
 *  The sine generation algorithm comes from Texas Instruments' "Digital
 *  Signal Processing Applications with the TMS320 Family" Application Book,
 *  volume 1, 1986, pp. 269-289 (Preceision Digital Sine-Wave Generation with
 *  the TMS32010).
 *
 *  The intent of the sine generation routine is to be able to produce stable,
 *  low distortion sine waves over a wide range of frequencies.
 */

/*
 *  The following is a sine table of length N (128) computed using the
 *  following code segment:
 *
 *      for ( i = 0; i < N; i++ )
 *	{
 *          tmp = 16384.0 * sin( 2 * PI * i / N );
 *          sineTable[i] = ( tmp >= 0 ) ? tmp + 0.5 : tmp - 0.5 ;
 *      }
 *
 *  As an implementation improvement, sine[0] is repeated at sine[128] for
 *  the slope calculation.
 */

unsigned short BSC_sineTable[] =
{
     0x0000, 0x0324, 0x0646, 0x0964, 0x0c7c, 0x0f8d, 0x1294, 0x1590,
     0x187e, 0x1b5d, 0x1e2b, 0x20e7, 0x238e, 0x2620, 0x289a, 0x2afb,
     0x2d41, 0x2f6c, 0x3179, 0x3368, 0x3537, 0x36e5, 0x3871, 0x39db,
     0x3b21, 0x3c42, 0x3d3f, 0x3e15, 0x3ec5, 0x3f4f, 0x3fb1, 0x3fec,
     0x4000, 0x3fec, 0x3fb1, 0x3f4f, 0x3ec5, 0x3e15, 0x3d3f, 0x3c42,
     0x3b21, 0x39db, 0x3871, 0x36e5, 0x3537, 0x3368, 0x3179, 0x2f6c,
     0x2d41, 0x2afb, 0x289a, 0x2620, 0x238e, 0x20e7, 0x1e2b, 0x1b5d,
     0x187e, 0x1590, 0x1294, 0x0f8d, 0x0c7c, 0x0964, 0x0646, 0x0324,
     0x0000, 0xfcdc, 0xf9ba, 0xf69c, 0xf384, 0xf073, 0xed6c, 0xea70,
     0xe782, 0xe4a3, 0xe1d5, 0xdf19, 0xdc72, 0xd9e0, 0xd766, 0xd505,
     0xd2bf, 0xd094, 0xce87, 0xcc98, 0xcac9, 0xc91b, 0xc78f, 0xc625,
     0xc4df, 0xc3be, 0xc2c1, 0xc1eb, 0xc13b, 0xc0b1, 0xc04f, 0xc014,
     0xc000, 0xc014, 0xc04f, 0xc0b1, 0xc13b, 0xc1eb, 0xc2c1, 0xc3be,
     0xc4df, 0xc625, 0xc78f, 0xc91b, 0xcac9, 0xcc98, 0xce87, 0xd094,
     0xd2bf, 0xd505, 0xd766, 0xd9e0, 0xdc72, 0xdf19, 0xe1d5, 0xe4a3,
     0xe782, 0xea70, 0xed6c, 0xf073, 0xf384, 0xf69c, 0xf9ba, 0xfcdc,
     0x0000
};

/*
 *  The following is the slope table of length N (128) computed using the
 *  following code segment:
 *
 *      for ( i = 0; i < N; i++ )
 *	{
 *
 *	    if ( i == ( N - 1 ) )
 *	    {
 *	        slopeTable[i] = sineTable[0] - sineTable[i];
 *	    }
 *	    else {
 *	        slopeTable[i] = sineTable[i + 1] - sineTable[i];
 *	    }
 *
 *      }
 */

unsigned short BSC_slopeTable[] =
{
     0x0324, 0x0322, 0x031e, 0x0318, 0x0311, 0x0307, 0x02fc, 0x02ee,
     0x02df, 0x02ce, 0x02bc, 0x02a7, 0x0292, 0x027a, 0x0261, 0x0246,
     0x022b, 0x020d, 0x01ef, 0x01cf, 0x01ae, 0x018c, 0x016a, 0x0146,
     0x0121, 0x00fd, 0x00d6, 0x00b0, 0x008a, 0x0062, 0x003b, 0x0014,
     0xffec, 0xffc5, 0xff9e, 0xff76, 0xff50, 0xff2a, 0xff03, 0xfedf,
     0xfeba, 0xfe96, 0xfe74, 0xfe52, 0xfe31, 0xfe11, 0xfdf3, 0xfdd5,
     0xfdba, 0xfd9f, 0xfd86, 0xfd6e, 0xfd59, 0xfd44, 0xfd32, 0xfd21,
     0xfd12, 0xfd04, 0xfcf9, 0xfcef, 0xfce8, 0xfce2, 0xfcde, 0xfcdc,
     0xfcdc, 0xfcde, 0xfce2, 0xfce8, 0xfcef, 0xfcf9, 0xfd04, 0xfd12,
     0xfd21, 0xfd32, 0xfd44, 0xfd59, 0xfd6e, 0xfd86, 0xfd9f, 0xfdba,
     0xfdd5, 0xfdf3, 0xfe11, 0xfe31, 0xfe52, 0xfe74, 0xfe96, 0xfeba,
     0xfedf, 0xff03, 0xff2a, 0xff50, 0xff76, 0xff9e, 0xffc5, 0xffec,
     0x0014, 0x003b, 0x0062, 0x008a, 0x00b0, 0x00d6, 0x00fd, 0x0121,
     0x0146, 0x016a, 0x018c, 0x01ae, 0x01cf, 0x01ef, 0x020d, 0x022b,
     0x0246, 0x0261, 0x027a, 0x0292, 0x02a7, 0x02bc, 0x02ce, 0x02df,
     0x02ee, 0x02fc, 0x0307, 0x0311, 0x0318, 0x031e, 0x0322, 0x0324
};

/*
 *  ======== BSC_AddSine ========
 *
 *  This routine generates a sine function given the amplitude, frequency, 
 *  and number of points to be generated.  Index is the running index through 
 *  the sine function (around the unit circle).  Because of the need to sum 
 *  up multiple sine waves (e.g. DTMF, MF, etc.), that data pointer is updated
 *  (+=) rather than just set.
 *
 *  The delta adjust value is the angle index given fixed point precision.  
 *  It is computed from the size of the sine table, the sampling frequency, 
 *  the integer/fraction representation of the delta value (S1.I7.F8), and 
 *  the final shift value to maximize precision.  Note, it is important to 
 *  round (vs. truncate) delta for best accuracy.
 */

#define DELTA_ADJUST	16777	/* tableSize / sampFreq * 256 * 4096 */

void BSC_AddSine( unsigned short ampl, unsigned short freq, int nPoints,
				  unsigned short *index, short *data )
{
    short	i;			/* loop counter */
    short	delta;		/* index adjustment value */
    short	value;		/* intermediate value */
    short	iIndex;		/* integer part of index */
    short	fIndex;		/* fractional part of index */ 
    short	slope;

    int		acc;		/* accumulator */

    acc = (int)freq * (int)DELTA_ADJUST + (int)( 1 << 11 );
    delta = (short)( acc >> 12 );

    for ( i = 0; i < nPoints; i++ )
    {
		iIndex = *index >> 8;
		fIndex = *index & 0xff;
		value = BSC_sineTable[iIndex];
		slope = BSC_slopeTable[iIndex];

		acc = (int)( (short)slope * (short)( fIndex << 4 & 0x0fff ) ); 
		*data++ += (short)( ( ( (short)value + (short)( acc >> 12 ) ) * ampl ) >> 14 );

		*index += delta;
		*index &= 0x7fff;
    }

    return;
}

unsigned short Average80( short *inBuffer )
{
    short	*ptr;
    unsigned short	max, tabs;
	unsigned int	absSum;

    /* find max peak, zero crossing, and mean values */
	max = 1;
	absSum = 0;

	for ( ptr = inBuffer; ptr < ( inBuffer + BSC_FRAME_SIZE ); ptr++ )
	{
	    /* take absolute value of current sample */
		tabs = BSC_abs( *ptr );

	    /* update absolue sum */
		absSum += tabs;

	    /* check for new maximum */
		if ( tabs > max )
        {
            max = tabs;
        }
	}

	return ( BSC_SignalLevel ) ( absSum >> BSC_MEAN_SHIFT );
}
