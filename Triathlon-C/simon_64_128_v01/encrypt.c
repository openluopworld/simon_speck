/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Daniel Dinu <dumitru-daniel.dinu@uni.lu>
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

/*
 * Simon_64_128_v01
 * encryption
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Encryption -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher encryption implementation */
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Encryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
    asm volatile (\
        /*---------------------------------------------------------------*/
	/* r4  - lower word of y					 */
        /* r5  - higher word of y		                         */
	/* r6  - lower word of x					 */
        /* r7  - higher word of x		                         */
	/* r8  - lower word of k					 */
        /* r9  - higher word of k		                         */
	/* r10 - 				                         */
	/* r11 - 				                         */
        /* r12 - temp1                                       		 */
        /* r13 - Loop counter                                            */
        /* r14 - RoundKeys i                                             */
        /* r15 - block                                           	 */
        /*---------------------------------------------------------------*/
        /* Store all modified registers                                  */
        /*---------------------------------------------------------------*/
	"push   r4;                 \n"
        "push   r5;                 \n"
        "push   r6;                 \n"
        "push   r7;                 \n"
        "push   r8;                 \n"
        "push   r9;                 \n"
        "push   r12;                \n"
        "push   r13;                \n"
        "push   r14;                \n"
        "push   r15;                \n"
        /*---------------------------------------------------------------*/
        "mov    %[block],       r15;\n"
        "mov    %[roundKeys],   r14;\n" 
        /*---------------------------------------------------------------*/
        /* load plain text	                                         */
        /*---------------------------------------------------------------*/
        "mov    @r15+,       	r4;\n"
        "mov    @r15+,       	r5;\n"
        "mov    @r15+,       	r6;\n"
        "mov    @r15+,       	r7;\n
        /*---------------------------------------------------------------*/
        "mov    #44,            r13;\n" /* 44 rounds                     */
"round_loop:\n"
        /* k = r9:r8;	*/ 
        "mov	@r14+,       	r8;\n"  
        "mov   	@r14+,        	r9;\n"
	/* k = k eor x */
        "eor	r6, 		r8;\n"
	"eor	r7,		r9;\n"
	/* x = y */
	"mov	r4,       	r6;\n"  
        "mov   	r5,        	r7;\n"
	/* S(8)(y) */
	/* A byte instruction with a register destination clears the high 8 bits of the register to 0. */
	/* [http://mspgcc.sourceforge.net/manual/x214.html] */
	/* I think the it means the destination regiser. */
  	"swpb r4;\n"
  	"swpb r5;\n"
	"mov.b r4, r12;\n"
  	"xor.b r5, r12;\n"
  	"xor  r12, r4;\n"
  	"xor  r12, r5;\n"
	/* S(1)x, This time x is store in y, so the operation is on y */
	"rla r4;\n"
	"rlc r5;\n"
	"adc r4;\n"
	/* Sx & S(8)x */
	"and	r4,		r6;\n"
	"and	r5,		r7;\n"
	/* S(2)x, again rotate shift left y with 1 bit*/
	"rla r4;\n"
	"rlc r5;\n"
	"adc r4;\n"
	/* [Sx & S(8)x] eor S(2)x */
	"eor	r4,		r6;\n"
	"eor	r5,		r7;\n"
	/* (y eor k) eor [Sx & S(8)x] eor S(2)x */
	"eor	r8,		r6;\n"
	"eor	r9,		r7;\n"

	/* loop control */
        "dec	r13;\n"
	"jne	round_loop;\n"
        /*---------------------------------------------------------------*/
        /* Restore registers                                             */
        /*---------------------------------------------------------------*/
        "pop    r15;                \n"
        "pop    r14;                \n"
        "pop    r13;                \n"
        "pop    r12;                \n"
        "pop    r9;                 \n"
        "pop    r8;                 \n"
        "pop    r7;                 \n"
        "pop    r6;                 \n"
        "pop    r5;                 \n"
	"pop    r4;                 \n"
        /*---------------------------------------------------------------*/
    :
    : [block] "m" (block), [roundKeys] "m" (roundKeys)
); 
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Encryption -- ARM			                                      */
/*----------------------------------------------------------------------------*/
/*
 * LSL provides the value of a register multiplied by a power of two. 
 * LSR provides the unsigned value of a register divided by a variable power of two.
 * Both instructions insert zeros into the vacated bit positions.
 *
 * ROR provides the value of the contents of a register rotated by a value.
 * The bits that are rotated off the right end are inserted into the vacated bit positions on the left
 */
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
    asm volatile (\
	/*--------------------------------------------------------------------*/
        /* r0  - X high byte to low byte                                      */
        /* r1  - Y high byte to low byte                                      */
        /* r2  - Temporary use                                                */
        /* r3  - Temporary use                                                */
        /* r4  - Temporary use                                                */
        /* r5  - Temporary use                                                */
        /* r6  - point of block                                               */
        /* r7  - point of roundKeys                                           */
        /* r8  - loop counter                                                 */
        /* r9  - round key                                                    */
        /*--------------------------------------------------------------------*/
        /* Store all modified registers                                       */
        /*--------------------------------------------------------------------*/
	"stmdb			sp!,			{r0-r12,lr};		\n" 
        "mov			r6, 			%[block]; 		\n" 
        "mov			r7, 			%[roundKeys]; 		\n"
	/* value of r6 will not change, if it's 'ldmia r6!,{r0-r1}', then r6 will add 8 after the instruction */
	"ldmia			r6, 			{r0-r1}; 		\n"
        "stmdb			sp!, 			{r6};			\n"
	"mov			r8,			#44;			\n"
"encrypt_round_loop:                           	   				\n"
	"ldmia			r7!, 			{r9}; 			\n"
	/* k eor y								*/
	"eor			r9,			r1, 		r9;	\n"
	/* y = x	                                                      	*/
	"mov			r1,			r0;			\n"
	/* x = S(1)x		                                         	*/
	"ands			r2,			r0,		#0x80000000;	\n"
	"lsl			r0,			#1;			\n"
	"orrne			r0,			r0, 		#0x1;	\n"
	/* t1 = S(1)x                                                      	*/
	"mov			r2,			r0;			\n"
	/* t2 = S(8)x								*/
	"mov			r3,			r1,		lsl#8;	\n"
	"mov			r4,			r1,		lsr#24;	\n"
	"eor			r3, 			r3,		r4;	\n"
	/* t1 = S(1)x & S(8)x							*/
	"and			r2,			r2,		r3;	\n"
	/* x = S(2)x , x rotate left with 1 bit again 				*/
	"ands			r3,			r0,		#0x80000000;	\n"
	"lsl			r0,			#1;			\n"
	"orrne			r0,			r0, 		#0x1;	\n"
	/* x eor t1 								*/
	"eor			r0, 			r0, 		r2;	\n"
	/* x eor k 								*/
	"eor			r0, 			r0, 		r9;	\n"
	/* loop control 							*/
	"subs			r8, 			r8, 		#1;	\n"
	"bne			encrypt_round_loop;				\n"
	/*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,			{r6}; 			\n"
        "stmia			r6,			{r0-r1};		\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,      {r0-r12,lr};           \n" /*                 */
        /*--------------------------------------------------------------------*/
	:
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	);
}

#else
/*----------------------------------------------------------------------------*/
/* Encryption -- Default c implementation                                     */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"

void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	uint8_t i;
	uint32_t k;
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *rightPtr = (uint32_t *)block;
	uint32_t *leftPtr = (rightPtr + 1);
	uint32_t tmp;

	for (i = 0; i < NUMBER_OF_ROUNDS; i++)
	{
		k = READ_ROUND_KEY_DOUBLE_WORD(rk[i]);
		tmp = *leftPtr;
		*leftPtr = f(*leftPtr) ^ *rightPtr ^ k;
		*rightPtr = tmp;
	}
}
#endif
#endif
#endif