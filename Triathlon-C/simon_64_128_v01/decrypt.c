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
 * decryption
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* decryption -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
    asm volatile (\
	/*------------------------------------------------------*/
        /* Registers allocation:				*/
        /* r0-r7   : cipher text 				*/
        /* r8-11   : round key					*/
        /* r12-r15 : temp use					*/
        /* r16-r23 :                            		*/
        /* r24     : currentRound				*/
        /* r25     : 						*/
        /* r26:r27 : X point to plain text			*/
        /* r28:r29 : Y roundKeys				*/
        /* r30:r31 : Z r30 stores 0				*/
        /* ---------------------------------------------------- */
        /* Store all modified registers				*/
        /* ---------------------------------------------------- */
	"push	r0;	\n" /* plain text */
        "push  	r1;	\n"
	"push	r2;	\n"
        "push  	r3;	\n"
        "push  	r4;	\n"
	"push  	r5;	\n"
	"push  	r6;	\n"
        "push  	r7;	\n"
        "push  	r8;	\n" /* round key */
	"push 	r9;	\n"
	"push  	r10;	\n"
        "push  	r11;	\n"
        "push  	r12;	\n" /* temp use */
	"push  	r13;	\n"
	"push  	r14;	\n"
	"push  	r15;	\n"
        "push 	r24;	\n" /* currentRound */
        "push 	r26;	\n" /* X */
        "push 	r27;	\n" 
        "push 	r28;	\n" /* Y */
        "push 	r29;	\n"
        "push 	r30;	\n" /* Z */
        "push 	r31;	\n"
	/* ---------------------------------------------------- */
/*	"ldi 		r26, 		lo8(block);		\n" */
/*	"ldi 		r27, 		hi8(block);		\n" */
	"clr 		r24;					\n"
/*	"ldi 		r28, 		lo8(roundKeys+176);	\n" */
/*	"ldi 		r29, 		hi8(roundKeys+176);	\n" */
/*	"adiw		r28,		176;			\n" */
	/* the second operand is between [0, 63] */
	"adiw		r28,		63;			\n"
	"adiw		r28,		63;			\n"
	"adiw		r28,		50;			\n"
	/* load the ciphertext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0] */
	"ld 		r7, 		x+ ;			\n"
	"ld 		r6, 		x+ ;			\n"
	"ld 		r5, 		x+ ;			\n"
	"ld 		r4, 		x+ ;			\n"
	"ld 		r3, 		x+ ;			\n"
	"ld 		r2, 		x+ ;			\n"
	"ld 		r1, 		x+ ;			\n"
	"ld 		r0, 		x+ ;			\n"
	"clr 		r30;					\n"
	/* ---------------------------------------------------- */
"decLoop:;							\n"
	/* get the sub key k, store the 4 bytes of sub key to K = [r11, r10, r9, r8] */
	"ld 		r11, 		-y ;			\n"
 	"ld 		r10, 		-y ;			\n"
	"ld 		r9, 		-y ;			\n"
	"ld 		r8, 		-y ;			\n"
	/* k = k eor x 						*/
	"eor 		r8, 		r4;			\n"
	"eor 		r9, 		r5;			\n"
	"eor 		r10, 		r6;			\n"
	"eor 		r11, 		r7;			\n"
	/* move y to x 						*/
	"movw 		r4, 		r0;			\n"
	"movw 		r6, 		r2;			\n"
	/* rotate y by left with 1 bit 				*/
	/* logical shift left: bit 0 is cleared, bit 7 is loaded into the C flag of the SREG */
	/* rotate left through carry: the C flag in shifted into bit 0, bit 7 is shifted into the C flag */
	"lsl 		r0;					\n"
	"rol 		r1;					\n"
	"rol 		r2;					\n"
	"rol 		r3;					\n"
	"adc 		r0, 		r30;			\n"
	/* move y to t, t stands for [r15, r14, r13, r12]	*/
	"movw 		r12, 		r0;			\n"
	"movw 		r14, 		r2;			\n"
	/* t & S8(y) 						*/
	"and 		r12, 		r7;			\n"
	"and 		r13, 		r4;			\n"
	"and 		r14, 		r5;			\n"
	"and 		r15, 		r6;			\n"
	/* y = S2(y) 						*/
	"lsl 		r0;					\n"
	"rol 		r1;					\n"
	"rol 		r2;					\n"
	"rol 		r3;					\n"
	"adc 		r0, 		r30;			\n"
	/* y = y eor t 						*/
	"eor 		r0, 		r12;			\n"
	"eor 		r1, 		r13;			\n"
	"eor 		r2, 		r14;			\n"
	"eor 		r3, 		r15;			\n"
	/* y = y eor k 						*/
	"eor 		r0, 		r8;			\n"
	"eor 		r1, 		r9;			\n"
	"eor 		r2, 		r10;			\n"
	"eor 		r3, 		r11;			\n"
	/* loop control 					*/
	"inc 		r24;					\n"
	"cpi 		r24, 		44;			\n"
	"brne 		decLoop;				\n"
	/* ---------------------------------------------------- */
	/* move cipher text back to plain text 			*/
	"st 		-x, 		r0;			\n"
	"st 		-x, 		r1;			\n"
	"st 		-x, 		r2;			\n"
	"st 		-x, 		r3;			\n"
	"st 		-x, 		r4;			\n"
	"st 		-x, 		r5;			\n"
	"st 		-x, 		r6;			\n"
	"st 		-x, 		r7;			\n"
	/* ---------------------------------------------------- */
	/* Restore all modified registers			*/
        "pop  r31;       \n"
        "pop  r30;       \n"
        "pop  r29;       \n"
        "pop  r28;       \n"
        "pop  r27;       \n" 
        "pop  r26;       \n" 
        "pop  r24;       \n" 
	"pop  r15;       \n"
	"pop  r14;       \n"
	"pop  r13;       \n"
        "pop  r12;       \n"
        "pop  r11;       \n"
	"pop  r10;       \n"
	"pop  r9;        \n"
	"pop  r8;        \n"
	"pop  r7;        \n"
	"pop  r6;        \n"
	"pop  r5;        \n"
	"pop  r4;        \n"
        "pop  r3;        \n"
        "pop  r2;        \n"
	"pop  r1;	 \n"
	"pop  r0;	 \n"
    :
    : [block] "x" (block), [roundKeys] "y" (roundKeys));
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* decryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
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
	"push   r4;                 	\n"
        "push   r5;                 	\n"
        "push   r6;                 	\n"
        "push   r7;                 	\n"
        "push   r8;                 	\n"
        "push   r9;                 	\n"
        "push   r12;                	\n"
        "push   r13;                	\n"
        "push   r14;                	\n"
        "push   r15;                	\n"
        /*--------------------------------------------------------------*/
        "mov    %[block],       r15;					\n"
        "mov    %[roundKeys],   r14;					\n"
	"add	#43*4,		r14;					\n" 
        /*--------------------------------------------------------------*/
        /* load cipher text		                                */
        /*--------------------------------------------------------------*/
    	"mov 	@r15+, r4;						\n"
    	"mov 	@r15+, r5;						\n"
    	"mov 	@r15+, r7;						\n"
    	"mov 	@r15+, r7;						\n"
        /*--------------------------------------------------------------*/
        "mov    #44,            r13;					\n" /* 44 rounds */
    "round_loop:							\n"
        /* k = r9:r8;							*/ 
        "mov	0(r14),       	r8;					\n"  
        "mov   	2(r14),        	r9;					\n"
	/* k = k eor y 							*/
        "xor	r4, 		r8;					\n"
	"xor	r5,		r9;					\n"
	/* y = x 							*/
	"mov	r6,       	r4;					\n"  
        "mov   	r7,        	r5;					\n"
	/* S(8)(x) 							*/
	/* A byte instruction with a register destination clears the high 8 bits of the register to 0. */
	/* [http://mspgcc.sourceforge.net/manual/x214.html] 		*/
	/* I think the it means the destination regiser. 		*/
  	"swpb 	r6;							\n"
  	"swpb 	r7;							\n"
	"mov.b 	r6, 		r12;					\n"
  	"xor.b 	r7, 		r12;					\n"
  	"xor  	r12, 		r6;					\n"
  	"xor  	r12, 		r7;					\n"
	/* S(1)y, This time y is store in x, so the operation is on x 	*/
	"rla 	r6;							\n"
	"rlc 	r7;							\n"
	"adc 	r6;							\n"
	/* Sy & S(8)y 							*/
	"and	r6,		r4;					\n"
	"and	r7,		r5;					\n"
	/* S(2)y, again rotate shift left x with 1 bit			*/
	"rla r6;							\n"
	"rlc r7;							\n"
	"adc r6;							\n"
	/* [Sy & S(8)y] eor S(2)y 					*/
	"xor	r6,		r4;					\n"
	"xor	r7,		r5;					\n"
	/* (x eor k) eor [Sy & S(8)y] eor S(2)y 			*/
	"xor	r8,		r4;					\n"
	"xor	r9,		r5;					\n"
	/* next k 							*/
	"sub	#4,		r14;					\n"
	/*---------------------------------------------------------------*/
	/* loop control 						*/
        "dec	r13;							\n"
	"jne	round_loop;						\n"
        /*--------------------------------------------------------------*/
        /* Restore registers                                             */
        /*--------------------------------------------------------------*/
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
    : [block] "" (block), [roundKeys] "" (roundKeys)); 
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* decryption -- ARM			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
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
	"ldmia			r6, 			{r0-r1}; 		\n"
        "stmdb			sp!, 			{r6};			\n"
	"mov			r8,			#44;			\n"
	/* end of keys 								*/
        "add           		r7,            		r7,        	#176;	\n"
"decrypt_round_loop:								\n"
        /* laod key 								*/
        "ldmdb			r7!,			{r9};             	\n"
	/* k = k eor x*/
        "eor			r9,			r0, 		r9;	\n"  
	/* x = y, move y to x 							*/
	"mov 			r0, 			r1;			\n"
	/* y = S(1)(y) 								*/
	"ands			r2,			r1,		#0x80000000;	\n"
	"lsl			r1,			#1;			\n"
	"orrne			r1,			r1, 		#0x1;	\n"
	/* t = S(1)y, move y to t 						*/
	"mov 			r5, 			r1;			\n"
	/* t = S(1)y and S(8)(y) 							*/
	"mov			r3,			r0,		lsl#8;	\n"
	"mov			r4,			r0,		lsr#24;	\n"
	"eor			r3, 			r3,		r4;	\n"
	"and			r5,			r5,		r3;	\n"
	/* y = S(2)(y) 								*/
	"ands			r2,			r1,		#0x80000000;	\n"
	"lsl			r1,			#1;			\n"
	"orrne			r1,			r1, 		#0x1;	\n"
	/* y =  [S(1)y and S(8)(y)] eor S(2)y 					*/
	"eor			r1,			r1,		r5;	\n"
	/* y =  [S(1)y and S(8)(y)] eor S(2)y eor k 				*/
	"eor			r1, 			r1,		r9;	\n"
	/* loop control 							*/
	"subs          		r8,           	 	r8,       	#1;	\n"
        "bne     	    	decrypt_round_loop;				\n" 
        /*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia        		sp!,             	{r6};           	\n"
        "stmia         		r6,          		{r0-r1};           	\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,      		{r0-r12,lr};           \n"
        /*--------------------------------------------------------------------*/
    :
    : [block] "" (block), [roundKeys] "" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* decryption -- Default c implementation                                     */
/*----------------------------------------------------------------------------*/
#include "primitives.h"

void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
    	int8_t i;
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *rightPtr = (uint32_t *)block;
    	uint32_t *leftPtr = (rightPtr + 1);
	uint32_t tmp;
	uint32_t k;

	for (i = NUMBER_OF_ROUNDS - 1; i >= 0; i--)
	{
		k = READ_ROUND_KEY_DOUBLE_WORD(rk[i]);
		tmp = *rightPtr;
		*rightPtr = f(*rightPtr) ^ *leftPtr ^ k;
		*leftPtr = tmp;
	}
}
#endif
#endif
#endif
