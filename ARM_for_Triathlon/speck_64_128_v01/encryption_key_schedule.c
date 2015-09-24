/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Yann Le Corre <yann.lecorre@uni.lu>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"


/*
 * Speck_64_128_v01
 * Key Schedule
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Key Schedule -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Key Schedule -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
        /*---------------------------------------------------------------*/
	/* r4  - lower word of ki					 */
        /* r5  - higher word of ki		                         */
	/* r6  - lower word of li					 */
        /* r7  - higher word of li		                         */
	/* r8  - lower word of l(i+1)					 */
        /* r9  - higher word of l(i+1)		                         */
	/* r10 - lower word of l(i+2)					 */
        /* r11 - higher word of l(i+2)		                         */
        /* r13 - Loop counter                                            */
        /* r14 - RoundKeys i                                             */
        /* r15 - Key                                                     */
        /*---------------------------------------------------------------*/
        /* Store all modified registers                                  */
        /*---------------------------------------------------------------*/
	"push   r4;                 \n"
        "push   r5;                 \n"
        "push   r6;                 \n"
        "push   r7;                 \n"
        "push   r8;                 \n"
        "push   r9;                 \n"
        "push   r10;                \n"
	"push   r11;                \n"
        "push   r13;                \n"
        "push   r14;                \n"
        "push   r15;                \n"
        /*--------------------------------------------------------------*/
        "mov    %[key],         r15;\n"
        "mov    %[roundKeys],   r14;\n" 
        /*--------------------------------------------------------------*/
        /* load master key						*/
        /*--------------------------------------------------------------*/
        "mov    @r15+,       	0(r14);\n" /* r15 will add 1 in word, but r14 in bytes. */ 
        "mov    @r15+,       	2(r14);\n"
        "mov    @r15+,       	r6;\n"
        "mov    @r15+,       	r7;\n"
        "mov    @r15+,       	r8;\n"
        "mov    @r15+,      	r9;\n"
        "mov    @r15+,      	r10;\n"
        "mov    @r15+,      	r11;\n"
        /*---------------------------------------------------------------*/
        "mov    #0,            r13;\n"
"round_loop:\n"
	/* S(-8)(li), ki is not used now */
	"mov.b	r6, 		r4;\n"
 	"xor.b	r7, 		r4;\n"
	"swpb	r6;\n"
	"swpb	r7;\n"
	"swpb	r4;\n"
	"xor	r4,		r6;\n"
	"xor	r4,		r7;\n"
        /* ki = r5:r4;	*/ 
        "mov	@r14+,       	r4;\n"  
        "mov   	@r14+,        	r5;\n" 
        /* li = ki + S(-8)(li);	*/
        "add	r4,		r6;\n"
	"add	r5,		r7;\n"
	/* li = [ki + S(-8)(li)] eor i */
        "eor   	r13,       	r6;\n"  
	/* ki = S(3)ki */
	"rla	r4;\n" /* S(-1) */
	"rlc	r5;\n"
	"adc	r4;\n"
	"rla	r4;\n" /* S(-1) */
	"rlc	r5;\n"
	"adc	r4;\n"
	"rla	r4;\n" /* S(-1) */
	"rlc	r5;\n"
	"adc	r4;\n"
	/* ki = S(3)ki eor [[ki + S(-8)(li)] eor i] */
	"eor	r6,		r4;\n"
	"eor	r7,		r5;\n"
	/* store ki+1. r14 has pointed to the address of ki+1 by instruction "@r14+" */
	"mov	r4,		0(r14);\n"
	"mov	r5,		2(r14);\n"
	/* update l */
	"mov	r6,		r4;\n" /* store li+3 */
	"mov	r7,		r5;\n"
	"mov	r8,		r6;\n" /* update li, covered by li+1 */
	"mov	r9,		r7;\n"
	"mov	r10,		r8;\n" /* update li+1, covered by li+2 */
	"mov	r11,		r9;\n"
	"mov	r4,		r10;\n" /* update li+2, covered by li+3 */
	"mov	r5,		r11;\n"
	/* loop control */
        "inc	r13;\n"
	"cmp	#26,		r13" /* 27 rounds, only need to schedule 26 rounds */
	"jne	round_loop;\n"
        /*---------------------------------------------------------------*/
        /* Restore registers                                             */
        /*---------------------------------------------------------------*/
        "pop    r15;                \n"
        "pop    r14;                \n"
        "pop    r13;                \n"
        "pop    r11;                \n"
	"pop    r10;                \n"
        "pop    r9;                 \n"
        "pop    r8;                 \n"
        "pop    r7;                 \n"
        "pop    r6;                 \n"
        "pop    r5;                 \n"
	"pop    r4;                 \n"
        /*---------------------------------------------------------------*/
    :
    : [key] "m" (key), [roundKeys] "m" (roundKeys), [Z_XOR_3] "" (Z_XOR_3)); 
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Key Schedule -- ARM			                                      */
/*----------------------------------------------------------------------------*/

/* key: [k0, l0, l1, l2]. The input should be opposite to the paper 	      */
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
        /*--------------------------------------------------------------------*/
        /* r0  - k[i]                                                         */
        /* r1  - l[i]                                            	      */
        /* r2  - l[i+1]                                                       */
        /* r3  - l[i+2]                                                       */
        /* r4  - temp                                                         */
        /* r5  - temp                                                         */
        /* r6  - point of master key                                          */
        /* r7  - point of round keys                                          */
        /* r8  - loop counter                                                 */
        /*--------------------------------------------------------------------*/
        /* Store all modified registers                                       */
        /*--------------------------------------------------------------------*/
	/* If the exclamation point following register was omitted, then the address register is not altered by the instruction */
        "stmdb        		sp!,   			{r0-r12,lr};            \n"
	/* initialize 								*/
        "mov          		r6,        		%[key];             	\n"
        "mov          		r7,  			%[roundKeys];           \n"
        "mov           		r8,           		#0;              	\n"
        /* master keys 								*/
        "ldmia        		r6!,      		{r0};              	\n"
	/* r7 is still the start address of round keys after the operation 	*/ 
	"stmia         		r7,      		{r0};             	\n" 
	"ldmia        		r6,      		{r1-r3};              	\n"
"key_schedule_loop:                                   				\n"
	/* load k[i] 								*/
	"ldmia			r7!,			{r0};			\n"
	/* l(i) = k(i) + S(-8)l(i)						 */
	"ror			r1, 			#8;			\n"
	"add 			r1,			r1,		r0;	\n"
	/* l(i) = l(i) eor currentRound 					*/
	"eor			r1,			r1,		r8;	\n"
	/* k(i) = S(3)k(i) 							*/
	"mov			r4,			r0,		lsl#3;	\n"
	"mov			r5,			r0,		lsr#29;	\n"
	"eor			r0, 			r4,		r5;	\n"
	/* k(i) = k(i) eor l(i), the result is k(i+1) 				*/
	"eor			r0,			r0,		r1;	\n"
	/* store k(i+1) 							*/
	"stmia         		r7,      		{r0};             	\n"
	/* update l */
	"mov			r4,			r1;			\n" /* store the new L  */
	"mov			r1,			r2;			\n"
	"mov			r2,			r3;			\n"
	"mov 			r3,			r4;			\n"
        /* loop control */
        "add          		r8,            		r8,          	#1;	\n"
	"cmp			r8,			#26;			\n" /* key schedule rounds is just 26(not 27) */
        "bne           	key_schedule_loop;              			\n" 
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        	sp!,      		{r0-r12,lr};           		\n"
        /*--------------------------------------------------------------------*/
    :
    : [key] "r" (key), [roundKeys] "r" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* Key Schedule -- Default C Implementation				      */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"

void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	uint8_t i;
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *mk = (uint32_t *)key;
	uint32_t lp3;
	uint32_t lp2;
	uint32_t lp1;
	uint32_t lp0;

	rk[0] = mk[0];
	for (i = 0; i < NUMBER_OF_ROUNDS - 1; ++i)
	{
        	if (i == 0)
        	{
          		lp0 = mk[1];
          		lp1 = mk[2];
			lp2 = mk[3]
       	 	}
       	 	else
        	{
            		lp0 = lp1;
            		lp1 = lp2;
			lp2 = lp3;
        	}
        	lp3 = (rorAlpha(lp0) + rk[i]) ^ i;
        	rk[i + 1] = rolBeta(rk[i]) ^ lp3;
    	}
}

#endif
#endif
#endif
