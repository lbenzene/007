/*
**********************************************************************************************************
     ___   _____    _____   _   _   _   __   _   _____  
    /   | |  _  \  |  _  \ | | | | | | |  \ | | /  _  \ 
   / /| | | |_| |  | | | | | | | | | | |   \| | | | | | 
  / / | | |  _  /  | | | | | | | | | | | |\   | | | | | 
 / /  | | | | \ \  | |_| | | |_| | | | | | \  | | |_| | 
/_/   |_| |_|  \_\ |_____/ \_____/ |_| |_|  \_| \_____/ 

**********************************************************************************************************
*/

#ifndef _OS_CPU_A_H_
#define _OS_CPU_A_H_

#include <avr/io.h>

/*
 * Macros for declaring a "naked" ISR: registers are not saved and restored and a ret command is missing.
 *
 */
//#define UCOSISR(signame)			\
//void signame (void) __attribute__ ((naked));	\
//void signame (void)

/*
 * Macros for PUSHing and POPping registers in application ISRs.
 *
 * Do not use PUSH_REGS(), PUSH_RAMPZ(), PUSH_SREG() directly.
 *
 * Use PUSHRS() and POPRS() in application programs.
 */

#define PUSH_REGS() \
        "push	r0	\n\t" \
        "push	r1	\n\t" \
        "push	r2	\n\t" \
        "push	r3	\n\t" \
        "push	r4	\n\t" \
        "push	r5	\n\t" \
        "push	r6	\n\t" \
        "push	r7	\n\t" \
        "push	r8	\n\t" \
        "push	r9	\n\t" \
        "push	r10	\n\t" \
        "push	r11	\n\t" \
        "push	r12	\n\t" \
        "push	r13	\n\t" \
        "push	r14	\n\t" \
        "push	r15	\n\t" \
        "push	r16	\n\t" \
        "push	r17	\n\t" \
        "push	r18	\n\t" \
        "push	r19	\n\t" \
        "push	r20	\n\t" \
        "push	r21	\n\t" \
        "push	r22	\n\t" \
        "push	r23	\n\t" \
        "push	r24	\n\t" \
        "push	r25	\n\t" \
        "push	r26	\n\t" \
        "push	r27	\n\t" \
        "push	r28	\n\t" \
        "push	r29	\n\t" \
        "push	r30	\n\t" \
        "push	r31	\n\t"  


#define POP_REGS() \
        "pop	r31	\n\t" \
        "pop	r30	\n\t" \
        "pop	r29	\n\t" \
        "pop	r28	\n\t" \
        "pop	r27	\n\t" \
        "pop	r26	\n\t" \
        "pop	r25	\n\t" \
        "pop	r24	\n\t" \
        "pop	r23	\n\t" \
        "pop	r22	\n\t" \
        "pop	r21	\n\t" \
        "pop	r20	\n\t" \
        "pop	r19	\n\t" \
        "pop	r18	\n\t" \
        "pop	r17	\n\t" \
        "pop	r16	\n\t" \
        "pop	r15	\n\t" \
        "pop	r14	\n\t" \
        "pop	r13	\n\t" \
        "pop	r12	\n\t" \
        "pop	r11	\n\t" \
        "pop	r10	\n\t" \
        "pop	r9	\n\t" \
        "pop	r8	\n\t" \
        "pop	r7	\n\t" \
        "pop	r6	\n\t" \
        "pop	r5	\n\t" \
        "pop	r4	\n\t" \
        "pop	r3	\n\t" \
        "pop	r2	\n\t" \
        "pop	r1	\n\t" \
        "pop	r0	\n\t" 


#ifdef RAMPZ
	#define PUSH_RAMPZ() \
        	"in	r16,0x3b	\n\t" \
        	"push	r16	\n\t" 

	#define POP_RAMPZ() \
        	"pop	r16	\n\t" \
        	"out	0x3b,r16	\n\t" 
#else
	#define PUSH_RAMPZ()
	#define POP_RAMPZ()
#endif


#ifdef EIND
	#define PUSH_EIND() \
        	"in	r16,0x3c	\n\t" \
        	"push	r16	\n\t" 

	#define POP_EIND() \
        	"pop	r16	\n\t" \
        	"out	0x3c,r16	\n\t" 

#else
	#define PUSH_EIND()
	#define POP_EIND()
#endif

/*
 * Macros for pushing and popping registers in application ISRs.
 *
 */
#define PUSH_ALL() \
		PUSH_REGS() \		
		PUSH_RAMPZ() \
		PUSH_EIND() 		

#define POP_ALL() \
		POP_EIND() \
		POP_RAMPZ() \
		POP_REGS() 		
		
		

#define	PUSH_SP() \
		"in	r28,__SP_L__	\n\t" \
		"st	z+,r28	\n\t" \
		"in	r29,__SP_H__	\n\t" \
		"st	z+,r29	\n\t" 

#define POP_SP() \
		"ld	r28,z+	\n\t" \
		"out	__SP_L__,r28	\n\t" \
		"ld	r29,z+	\n\t" \
		"out	__SP_H__,r29	\n\t" 

#define PUSH_SREG() \
		"in	r16,__SREG__	\n\t" \
		"push	r16	\n\t" 

#define POP_SREG() \
		"pop	r16	\n\t" \
		"out	__SREG__,r16	\n\t" 


#define PUSH_SREG_ISR() \
		"in	r16,__SREG__	\n\t" \
		"sbr	r16,0x80	\n\t" \
		"push	r16	\n\t" 

#define POP_SREG_ISR() \
		"pop	r16	\n\t" \
		"cbr	r16,0x80	\n\t" \	
		"out	__SREG__,r16	\n\t" 

#endif