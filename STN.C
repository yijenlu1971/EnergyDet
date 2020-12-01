/*
 *  ======== stn.c ========
 *
 *  Silence/Tone Detection Module
 *
 *  Copyright(c) 1996-1996 by AltiGen Communications, Inc.  All rights reserved.
 */

#include <Windows.h>
#include "bsc.h"
#include "stn.h" 

/* default configurations (using 10 mSec frames) */
STN_Config STN_config =
{
	FALSE,		/* noise detection */
    100,		/* pauseTimeout (frames) */
    700,		/* silenceTimeout (frames) */
    1300,		/* dialtoneTimeout (frames) */
    500,		/* reorderTimeout (frames) */
    IP_TRK_SILENCE_THRESHOLD,	/* silence threshold (dbm) */
    VOICE_LEVEL
};

/*
 *  ======== STN_Init ========
 *
 *  Purpose:
 *	Initialize module context for each recording session.
 *
 *  Return value:
 *	None.
 */
void STN_Init( STN_Context *ctx, STN_Config *config )
{
    unsigned short	target;

    /* initialize context to zero */
	RtlZeroMemory( ctx, sizeof( STN_Context ) );

    /* copy configuration information */
	if( config )
		RtlCopyMemory( &ctx->config, config, sizeof(STN_Config) );
	else
		RtlCopyMemory( &ctx->config, &STN_config, sizeof(STN_Config) );
	
	target = ((ctx->config.silenceThreshold * 64) / 51) >> 2;
	ctx->config.voiceThreshold = target;
	if( ctx->config.voiceThreshold > VOICE_LEVEL )
		ctx->config.voiceThreshold = VOICE_LEVEL;
	
    /* set to debounce state to fill up the signal history */
	ctx->state = SIG_TONE_DOWN;

	ctx->floorMean = NO_TONE_THRESHOLD;
}

/*
 *  ======== STN_Detect ========
 *
 *  Purpose:
 *	To determine the states in the input data, which are used to terminate
 *	the recording.  The detection include silence, dialtone, and reorder
 *	tone.  The silence is detected through the silence threshold.  The
 *	tone levels of dialtone and reorder tone are detected by comparing with
 *	the minimum/maximum reference levels.
 *
 *  Return value:
 *	None.
 */
void STN_Detect( STN_Context *ctx, unsigned short mean )
{
    STN_Config		*cfg = &ctx->config;
    unsigned short	target;

    /* update filtered mean and signal history */
	ctx->sumMean = ctx->sumMean + mean - ctx->sigMean[ctx->index];
	ctx->sigMean[ctx->index] = mean;

	if( ++ctx->index >= 4 )
	{
	    ctx->index = 0;
	}

	/* if trunk connected ?, if no, perform detection */
	if( !(ctx->results & STN_VOICE) )
	{
		if( ctx->sumMean >= CONNECTED_THRESHOLD )
		{
		    ctx->cntSound++;
		}
		else
		{
	 	   ctx->cntSound = 0;
		}
	}

    /* update current state based on tests */
	switch ( ctx->state )
	{
	case SIG_SILENCE:	// silence detected

		// search the min threshold of noise (-48dBm), too low become dead silence
		if( !cfg->SilenceDT )
		{
			if( mean >= BSC_N48_DBM )
				ctx->cntSteady++;
			else
				ctx->cntSteady = 0;
			
			if( (ctx->cntSteady >= STEADY_DELAY) &&	(ctx->floorMean > ctx->sumMean) )
			{
				ctx->floorMean = ctx->sumMean;
	
				// silence threshold above 4dB of min noise floor
				target = (ctx->floorMean * 256) / 161;
				if( (target>>2) != (cfg->silenceThreshold>>2) )
				{
					cfg->silenceThreshold = target;
					
					// voice threshold above 2dB of silence threshold
					target = ((target * 64) / 51) >> 2;
					if( target > VOICE_LEVEL ) 
						target = VOICE_LEVEL;
					cfg->voiceThreshold = target;
					
					// recount silence while threshold changed
					ctx->cntSilence = 0;
				}
			}
		}
		
	    ctx->cntNoTone++;

	    if( ctx->sumMean <= cfg->silenceThreshold )
	    {
			ctx->cntSilence++;
			ctx->cntDebounce = 0;
	    }
	    else
	    {
			if( ctx->sumMean > NO_TONE_THRESHOLD )
			{
			    ctx->state = SIG_TONE_UP;
			    ctx->cntDebounce = 0;
	
		        if( !ctx->tone )
			    {
					ctx->tone = TRUE;
					ctx->cntReorder = 0;
					ctx->refCount = 0;
					ctx->cntDuration = 0;
			    }
			    else
					ctx->cntDuration = ctx->cntNoTone;
	
			    ctx->cntNoTone = 0;
				ctx->cntSteady = 0;
				ctx->cntSilence = 0;
			}
	    }
	    break;

	case SIG_TONE_UP:	// debounce for tone coming up
	    if( ++ctx->cntDebounce >= TONE_DEBOUNCE )
	    {
			if( ctx->sumMean >= LOWER_TONE_THRESHOLD )
			{
			    ctx->state = SIG_TONE;
			    ctx->cntTone = 0;
	
			    ctx->refMean = ( ctx->sumMean >> 2 );
			    ctx->cntDuration += ctx->cntDebounce;

			    if( ctx->tone &&
					(ctx->refCount == 0 || ReorderDuration(ctx)) )
			    {
					ctx->cntReorder += ctx->cntDuration;
			    }
			    else
					ctx->tone = FALSE;
			}
			else if( ctx->sumMean > cfg->silenceThreshold )
			{
			    ctx->state = SIG_VOICE;
			    ctx->cntDebounce = 0;
			}
			else
			{
			    ctx->state = SIG_SILENCE;
			    ctx->tone = FALSE;
			}
	    }
	    break;

	case SIG_TONE:	// tone present
	    if ( ToneLevel(ctx) )
	    {
			ctx->cntTone++;
	    }
	    else if( ctx->sumMean < ( ctx->refMean * 4 ) )
	    {
			ctx->state = SIG_TONE_DOWN;
			ctx->cntDebounce = 0;

			/* if reference count is not set */
			if( ctx->tone && ctx->refCount == 0 )
			{
				ctx->refCount = ctx->cntTone + TONE_DEBOUNCE;
			}

			ctx->cntDuration = ctx->cntTone;
			ctx->cntTone = 0;
	    }
	    else
	    {
			ctx->state = SIG_VOICE;
			ctx->cntDebounce = 0;
	    }
	    break;

	case SIG_TONE_DOWN:	// debounce for tone going down
	    if( ++ctx->cntDebounce >= TONE_DEBOUNCE )
	    {
			if( ctx->sumMean > NO_TONE_THRESHOLD )
			{
			    ctx->state = SIG_VOICE;
			}
			else
			{
			    ctx->state = SIG_SILENCE;
			    ctx->cntDuration += ctx->cntDebounce;
	
			    if( ctx->tone && ReorderDuration(ctx) )
			    {
					ctx->cntReorder += ctx->cntDuration;
			    }
			    else
					ctx->tone = FALSE;
			}
			ctx->cntDebounce = 0;
	    }
	    break;

	case SIG_VOICE:	// voice detected

	    ctx->tone = FALSE;

	    if( ++ctx->cntDebounce >= SYNC_DELAY )
	    {
			ctx->cntDebounce = 0;
			if( ctx->sumMean > NO_TONE_THRESHOLD )
			{
			    ctx->state = SIG_TONE_UP;
			}
			else
			{
			    ctx->state = SIG_SILENCE;
			}
	    }
	    break;

	default:	// default should never happen
	    break;	
	}

    /* reset reorder tone counter */
	if( ctx->state == SIG_SILENCE && !ctx->tone )
	{
	    ctx->cntReorder = 0;
	}

    /* reset silence counter in pause state if signal level is ascending */
	if( (ctx->results & STN_PAUSE) && (mean > cfg->voiceThreshold) )
	{
	    ctx->cntSilence = 0;
	}

    /* check if pause, silence, dialtone, or reorder tone is detected */
	ctx->results = 0;

	if( ctx->cntSilence >= cfg->pauseTimeout )
	{
	    ctx->results |= STN_PAUSE;
	}

	if( ctx->cntSilence >= cfg->silenceTimeout )
	{
	    ctx->results |= STN_SILENCE;
	}

	if( ctx->cntTone >= cfg->dialtoneTimeout )
	{
	    ctx->results |= STN_DIALTONE;
	}

	if( ctx->cntReorder >= cfg->reorderTimeout )
	{
	    ctx->results |= STN_REORDER;
	}

    /* check if trunk is in connected state */
	if( ctx->cntSound > MIN_DURATION )
	{
	    ctx->results |= STN_VOICE;
	}
}
