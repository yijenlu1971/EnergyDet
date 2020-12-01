/*
 *  ======== stn.h ========
 *
 *  Silence/Tone Detection Module
 *
 *  Copyright(c) 1996-1996 by AltiGen Communications Inc.  All rights reserved.
 */

#ifndef STN_H_
#define STN_H_

#include "bsc.h"

/* definitions */

#define	UPPER_TONE_THRESHOLD		(4*BSC_P10_DBM)
#define	LOWER_TONE_THRESHOLD		(4*BSC_N20_DBM)
#define	NO_TONE_THRESHOLD			(4*BSC_N30_DBM)
#define	CONNECTED_THRESHOLD			(4*BSC_N30_DBM)
#define	IP_TRK_SILENCE_THRESHOLD	(4*BSC_N48_DBM)
#define IP_EXT_SILENCE_THRESHOLD	(4*BSC_N42_DBM)
#define	NOISE_THRESHOLD				(4*BSC_N36_DBM)

#define	VOICE_LEVEL					BSC_N40_DBM

#define	MIN_FACTOR			3
#define	MAX_FACTOR			5

#define STEADY_DELAY		6
#define	SYNC_DELAY			3
#define	TONE_DEBOUNCE		6
#define	MIN_DURATION		10
#define	MAX_DURATION		80

/* result bit definitions */

typedef unsigned short STN_Results;

#define STN_PAUSE			0x0001
#define STN_SILENCE			0x0002
#define STN_DIALTONE		0x0004
#define STN_REORDER			0x0008
#define STN_VOICE		    0x0010

#define STN_RESULT_MASK		0x000F

/* signal threshold macros */
#define ToneLevel(ctx)							\
    ( ctx->sumMean >= LOWER_TONE_THRESHOLD &&				\
      ctx->sumMean <= UPPER_TONE_THRESHOLD &&				\
      ctx->sumMean >= ((ctx->refMean<<2)-(ctx->refMean>>1)) &&		\
      ctx->sumMean <= ((ctx->refMean<<2)+(ctx->refMean>>1)) )

#define ToneLevel2(ctx)							\
    ( ctx->sumMean >= LOWER_TONE_THRESHOLD &&				\
      ctx->sumMean <= UPPER_TONE_THRESHOLD &&				\
      ctx->sumMean >= (ctx->refMean*MIN_FACTOR) &&			\
      ctx->sumMean <= (ctx->refMean*MAX_FACTOR) )

#define ReorderDuration(ctx)						\
    ( ctx->cntDuration >= MIN_DURATION &&				\
      ctx->cntDuration <= MAX_DURATION &&				\
     (ctx->cntDuration*4) >= (ctx->refCount*MIN_FACTOR) &&		\
     (ctx->cntDuration*4) <= (ctx->refCount*MAX_FACTOR) )

/* signal state enumeration */
typedef enum
{
    SIG_SILENCE = 0,		/* silence present */
    SIG_TONE_UP,			/* tone coming up */
    SIG_TONE,				/* tone present */
    SIG_TONE_DOWN,			/* tone going down */
    SIG_VOICE				/* voice present */
} STN_State;

/* configuration structure */
typedef struct
{
	unsigned short	SilenceDT;		/* silence or noise detect */
    unsigned short	pauseTimeout;	/* pause threshold (frames) */
    unsigned short	silenceTimeout;	/* silence threshold (frames) */
    unsigned short	dialtoneTimeout;/* dialtone threshold (frames) */
    unsigned short	reorderTimeout;	/* reorder threshold (frames) */
    unsigned short	silenceThreshold;	/* silence threshold (dbm) */
    unsigned short	voiceThreshold;	/* voice threshold */
} STN_Config;

/* context structure (56 bytes) */
typedef struct
{
    STN_State		state;		/* signal state */
    STN_Results		results;	/* test results (bitwise) */
    STN_Config		config;		/* module configuration */
    unsigned short	cntSilence;	/* silence count (in frames) */
    unsigned short	cntNoTone;	/* no signal count (frames) */
    unsigned short	cntTone;	/* tone count (frames) */
    unsigned short	cntDuration;	/* signal duration (frames) */
    unsigned short	cntDebounce;	/* tone debounce count (frames) */
    unsigned short	cntReorder;	/* reorder tone count (frames) */
    unsigned short	cntSound;	/* any sound count (frames) */
    unsigned short	refCount;	/* tone reference count */
    unsigned short	refMean;	/* tone reference mean */
    unsigned short	sumMean;	/* current filtered mean */
    unsigned short	index;		/* index of signal history */
    unsigned short	sigMean[4];	/* signal history */
	unsigned short	floorMean;
	unsigned short	cntSteady;
    unsigned short	tone;		/* tone in progress */
} STN_Context;

/* variable declarations */
extern STN_Config STN_config;

/* function prototypes */
extern void STN_Init( STN_Context *ctx, STN_Config *config );
extern void STN_Detect( STN_Context *ctx, unsigned short mean );

#endif /* STN_H_ */
