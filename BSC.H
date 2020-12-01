/*
 *  ======== bsc.h ========
 *
 *  Basic Analysis Module
 *
 *  Copyright (c) 1995-1996 by QIC:Solutions Inc.  All rights reserved.
 */

#ifndef BSC_H_
#define BSC_H_


/* general definitions and macros */
#define BSC_FRAME_SIZE		80
#define BSC_FRAME_TIME		10
#define BSC_MEAN_SHIFT		6

#define BSC_abs(x)		(((x) >= 0) ? (x) : -(x))
#define BSC_max(x,y)	(((x) > (y)) ? (x) : (y))

/* type definition for use with module mean values */
typedef unsigned short	BSC_SignalLevel;

#define ITU_ZERO_DBM	5776    /* CCITT digital 0 dBm (peak) */

#define ITU_P10_DBM	18265
#define ITU_P09_DBM	16279
#define ITU_P08_DBM	14509
#define ITU_P07_DBM	12931
#define ITU_P06_DBM	11525
#define ITU_P05_DBM	10271
#define ITU_P04_DBM	9154
#define ITU_P03_DBM	8159
#define ITU_P02_DBM	7272
#define ITU_P01_DBM	6481
#define ITU_Z00_DBM	5776
#define ITU_N01_DBM	5148
#define ITU_N02_DBM	4588
#define ITU_N03_DBM	4089
#define ITU_N04_DBM	3644
#define ITU_N05_DBM	3248
#define ITU_N06_DBM	2895
#define ITU_N07_DBM	2580
#define ITU_N08_DBM	2299
#define ITU_N09_DBM	2049
#define ITU_N10_DBM	1827
#define ITU_N11_DBM	1628
#define ITU_N12_DBM	1451
#define ITU_N13_DBM	1293
#define ITU_N14_DBM	1152
#define ITU_N15_DBM	1027
#define ITU_N16_DBM	915
#define ITU_N17_DBM	816
#define ITU_N18_DBM	727
#define ITU_N19_DBM	648
#define ITU_N20_DBM	578
#define ITU_N21_DBM	515
#define ITU_N22_DBM	459
#define ITU_N23_DBM	409
#define ITU_N24_DBM	364
#define ITU_N25_DBM	325
#define ITU_N26_DBM	289
#define ITU_N27_DBM	258
#define ITU_N28_DBM	230
#define ITU_N29_DBM	205
#define ITU_N30_DBM	183
#define ITU_N31_DBM	163
#define ITU_N32_DBM	145
#define ITU_N33_DBM	129
#define ITU_N34_DBM	115
#define ITU_N35_DBM	103
#define ITU_N36_DBM	92
#define ITU_N37_DBM	82
#define ITU_N38_DBM	73
#define ITU_N39_DBM	65
#define ITU_N40_DBM	58
#define ITU_N41_DBM	51
#define ITU_N42_DBM	46
#define ITU_N43_DBM	41
#define ITU_N44_DBM	36
#define ITU_N45_DBM	32
#define ITU_N46_DBM	29
#define ITU_N47_DBM	26
#define ITU_N48_DBM	23
#define ITU_N49_DBM	20
#define ITU_N50_DBM	18
#define ITU_N51_DBM	16
#define ITU_N52_DBM	15
#define ITU_N53_DBM	13
#define ITU_N54_DBM	12
#define ITU_N55_DBM	10
#define ITU_N56_DBM	9
#define ITU_N57_DBM	8
#define ITU_N58_DBM	7
#define ITU_N59_DBM	6
#define ITU_N60_DBM	6
#define ITU_N99_DBM	0

/* tip/ring dBm level values (codec dependent) */

#define BSC_P10_DBM		8197
#define BSC_P09_DBM		7305
#define BSC_P08_DBM		6511
#define BSC_P07_DBM		5803
#define BSC_P06_DBM		5172
#define BSC_P05_DBM		4609
#define BSC_P04_DBM		4108
#define BSC_P03_DBM		3661
#define BSC_P02_DBM		3263
#define BSC_P01_DBM		2908
#define BSC_Z00_DBM		2592
#define BSC_N01_DBM		2310
#define BSC_N02_DBM		2059
#define BSC_N03_DBM		1835
#define BSC_N04_DBM		1635
#define BSC_N05_DBM		1458
#define BSC_N06_DBM		1299
#define BSC_N07_DBM		1158
#define BSC_N08_DBM		1032
#define BSC_N09_DBM		920
#define BSC_N10_DBM		820
#define BSC_N11_DBM		731
#define BSC_N12_DBM		651
#define BSC_N13_DBM		580
#define BSC_N14_DBM		517
#define BSC_N15_DBM		461
#define BSC_N16_DBM		411
#define BSC_N17_DBM		366
#define BSC_N18_DBM		326
#define BSC_N19_DBM		291
#define BSC_N20_DBM		259
#define BSC_N21_DBM		231
#define BSC_N22_DBM		206
#define BSC_N23_DBM		183
#define BSC_N24_DBM		164
#define BSC_N25_DBM		146
#define BSC_N26_DBM		130
#define BSC_N27_DBM		116
#define BSC_N28_DBM		103
#define BSC_N29_DBM		92
#define BSC_N30_DBM		82
#define BSC_N31_DBM		73
#define BSC_N32_DBM		65
#define BSC_N33_DBM		58
#define BSC_N34_DBM		52
#define BSC_N35_DBM		46
#define BSC_N36_DBM		41
#define BSC_N37_DBM		37
#define BSC_N38_DBM		33
#define BSC_N39_DBM		29
#define BSC_N40_DBM		26
#define BSC_N41_DBM		23
#define BSC_N42_DBM		21
#define BSC_N43_DBM		18
#define BSC_N44_DBM		16
#define BSC_N45_DBM		15
#define BSC_N46_DBM		13
#define BSC_N47_DBM		12
#define BSC_N48_DBM		10
#define BSC_N49_DBM		9
#define BSC_N50_DBM		8
#define BSC_N51_DBM		7
#define BSC_N52_DBM		7
#define BSC_N53_DBM		6
#define BSC_N54_DBM		5
#define BSC_N55_DBM		5
#define BSC_N56_DBM		4
#define BSC_N57_DBM		4
#define BSC_N58_DBM		3
#define BSC_N59_DBM		3
#define BSC_N60_DBM		3
#define BSC_N99_DBM		0

/* context structure (24 bytes) */
typedef struct
{
    unsigned int	absSum;		/* absolute sum <-- changed to int _FT */
    unsigned short	max;		/* maximum value */
    BSC_SignalLevel	mean;		/* mean value */
    int				zCross;		/* number of zero crossings */
    short			upFactor;	/* scale up factor */
    short			downFactor;	/* scale down factor */
	/* NOTE: downFactor must be immediately	** after upFactor! */
    unsigned short	lastIsPos;	/* last buffer's last value pos */
    unsigned short	init;		/* module initialization flag */
} BSC_Context;

/* external declarations */
extern unsigned short BSC_sineTable[];

/* function prototypes */
int BSC_Init( BSC_Context *context );
int BSC_Analyze( BSC_Context *context, short *inBuffer, short *outBuffer );
void BSC_AddSine( unsigned short ampl, unsigned short freq, int nPoints, unsigned short *index, short *data );
unsigned short Average80( short *inBuffer );

#endif /* BSC_H_ */
