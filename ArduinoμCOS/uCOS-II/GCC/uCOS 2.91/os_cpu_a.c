
#include <avr/io.h>
#include <avr/interrupt.h>
#include "os_cpu_a.h"
#include "ucos_ii.h"

/********************************************************************************************************
*                            DISABLE/ENABLE INTERRUPTS USING OS_CRITICAL_METHOD #3
*
* Description : These functions are used to disable and enable interrupts using OS_CRITICAL_METHOD #3.
*
*               OS_CPU_SR  OSCPUSaveSR (void)
*                     Get current value of SREG
*                     Disable interrupts
*                     Return original value of SREG
*
*               void  OSCPURestoreSR (OS_CPU_SR cpu_sr)
*                     Set SREG to cpu_sr
*                     Return
*********************************************************************************************************/
OS_CPU_SR	OS_CPU_SR_Save(void)
{
	__asm__ __volatile__(
	"in	r24,__SREG__	\n\t"
	"cli	\n\t"
	"ret	\n\t"
	::);
}

void	OS_CPU_SR_Restore(OS_CPU_SR  cpu_sr)
{
	__asm__ __volatile__(
	"out	__SREG__,r24	\n\t"
	"ret	\n\t"
	::);
}

/********************************************************************************************************
*                               START HIGHEST PRIORITY TASK READY-TO-RUN
*
* Description : This function is called by OSStart() to start the highest priority task that was created
*               by your application before calling OSStart().
*
* Note(s)     : 1) The (data)stack frame is assumed to look as follows:
*
*                  OSTCBHighRdy->OSTCBStkPtr --> LSB of (return) stack pointer           (Low memory)
*                                                SPH of (return) stack pointer
*                                                Flags to load in status register
*                                                R31
*                                                R30
*                                                R7
*                                                .
*                                                .
*                                                .
*                                                R0
*                                                PCH
*                                                PCL                                     (High memory)
*
*                  where the stack pointer points to the task start address.
*
*
*               2) OSStartHighRdy() MUST:
*                      a) Call OSTaskSwHook() then,
*                      b) Set OSRunning to TRUE,
*                      c) Switch to the highest priority task.
*********************************************************************************************************/
void	OSStartHighRdy(void)
{
	__asm__ __volatile__(
	"call	OSTaskSwHook	\n\t"		// Invoke user defined context switch hook

	"lds	r16,OSRunning	\n\t"		// Indicate that we are multitasking
	"inc	r16	\n\t"
	"sts	OSRunning,r16	\n\t"

	"lds	r30,OSTCBHighRdy	\n\t"	// Let Z point to TCB of highest priority task
	"lds	r31,OSTCBHighRdy+1	\n\t"
	
	POP_SP()				// Load stack  pointer
	POP_SREG_ISR()			// Pop all registers
	POP_ALL()

	"reti	\n\t"				// Start task
	::);
}

/********************************************************************************************************
*                                       TASK LEVEL CONTEXT SWITCH
*
* Description : This function is called when a task makes a higher priority task ready-to-run.
*
* Note(s)     : 1) Upon entry,
*                  OSTCBCur     points to the OS_TCB of the task to suspend
*                  OSTCBHighRdy points to the OS_TCB of the task to resume
*
*               2) The stack frame of the task to suspend looks as follows:
*
*                                       SP+0 --> LSB of task code address
*                                         +1     MSB of task code address                (High memory)
*
*               3) The saved context of the task to resume looks as follows:
*
*                  OSTCBHighRdy->OSTCBStkPtr --> Flags to load in status register (Low memory)
*                                                R31
*                                                R30
*                                                R7
*                                                .
*                                                .
*                                                .
*                                                R0
*                                                PCH
*                                                PCL                                     (High memory)
*********************************************************************************************************/
void	OSCtxSw(void)
{
	__asm__ __volatile__(
	PUSH_ALL()					// Save current tasks context
	PUSH_SREG()					// Save SREG

	"lds	r30,OSTCBCur	\n\t"			// Z = OSTCBCur->OSTCBStkPtr
	"lds	r31,OSTCBCur+1	\n\t"

	PUSH_SP()					// Save Y (R29:R28) pointer
	
	"call	OSTaskSwHook	\n\t"			// Call user defined task switch hook

	"lds	r16,OSPrioHighRdy	\n\t"		// OSPrioCur = OSPrioHighRdy
	"sts	OSPrioCur,r16	\n\t"

	"lds	r30,OSTCBHighRdy	\n\t"		// Let Z point to TCB of highest priority task
	"lds	r31,OSTCBHighRdy+1	\n\t"		// ready to run
	"sts	OSTCBCur,r30	\n\t"			// OSTCBCur = OSTCBHighRdy
	"sts	OSTCBCur+1,r31	\n\t"
	
	POP_SP()					// Restore Y pointer

	"pop	r16	\n\t"				// Restore status register
	"sbrc	r16,7	\n\t"				// Skip next instruction in interrupts DISABLED
	"rjmp	OSCtxSw_1	\n\t"
	"out	__SREG__,r16	\n\t"			// Interrupts of task to return to are DISABLED	
	POP_ALL()
	"ret	\n\t"
	
	"OSCtxSw_1:	\n\t"
	"cbr	r16,0x80	\n\t"			// Interrupts of task to return to are ENABLED
	"out	__SREG__,r16	\n\t"	
	POP_ALL()					// Restore all registers
	"reti	\n\t"
	::);		
}

/*********************************************************************************************************
*                                INTERRUPT LEVEL CONTEXT SWITCH
*
* Description : This function is called by OSIntExit() to perform a context switch to a task that has
*               been made ready-to-run by an ISR.
*
* Note(s)     : 1) Upon entry,
*                  OSTCBCur     points to the OS_TCB of the task to suspend
*                  OSTCBHighRdy points to the OS_TCB of the task to resume
*
*               2) The stack frame of the task to suspend looks as follows:
*
*                  OSTCBCur->OSTCBStkPtr ------> Flags to load in status register        (Low memory)
*                                                R31
*                                                R30
*                                                R27
*                                                .
*                                                .
*                                                R0
*                                                PCH
*                                                PCL                                     (High memory)
*
*               3) The saved context of the task to resume looks as follows:
*
*                  OSTCBHighRdy->OSTCBStkPtr --> Flags to load in status register         (Low memory)
*                                                R31
*                                                R30
*                                                R7
*                                                .
*                                                .
*                                                .
*                                                R0                                      (High memory)
**********************************************************************************************************/
void	OSIntCtxSw(void)
{
	__asm__ __volatile__(
	"call	OSTaskSwHook	\n\t"			// Call user defined task switch hook
	
	"lds	r16,OSPrioHighRdy	\n\t"		// OSPrioCur = OSPrioHighRdy
	"sts	OSPrioCur,r16	\n\t"

	"lds	r30,OSTCBHighRdy	\n\t"		// Z = OSTCBHighRdy->OSTCBStkPtr
	"lds	r31,OSTCBHighRdy+1	\n\t"
	"sts	OSTCBCur,r30	\n\t"			// OSTCBCur = OSTCBHighRdy
	"sts	OSTCBCur+1,r31	\n\t"
	
	POP_SP()					// Restore Y pointer

	"pop	r16	\n\t"				// Restore status register
	"sbrc	r16,7	\n\t"				// Skip next instruction in interrupts DISABLED
	"rjmp	OSIntCtxSw_1	\n\t"
	"out	__SREG__,r16	\n\t"			// Interrupts of task to return to are DISABLED
	POP_ALL()
	"ret	\n\t"
	
	"OSIntCtxSw_1:	\n\t"
	"cbr	r16,0x80	\n\t"			// Interrupts of task to return to are ENABLED
	"out	__SREG__,r16	\n\t"
	POP_ALL()					// Restore all registers
	"reti	\n\t"
	::);	
}


/********************************************************************************************************
*                                           SYSTEM TICK ISR
*
* Description : This function is the ISR used to notify uC/OS-II that a system tick has occurred.
*
* In order to the corresponding 
* interrupt vector to be set to point to TIMER0_OVF_vect, one has to also #include <avr/io.h> in this file. 
* Note that no error message or warning will be given, if the include is not present, the interupt just
* won't get vectored.
*
*********************************************************************************************************/
ISR(TIMER0_OVF_vect, ISR_NAKED)
{
	__asm__ __volatile__(
	PUSH_ALL()							// Save all registers
	PUSH_SREG_ISR()						// Save SREG with I bit set!
	
	"lds	r16,OSIntNesting	\n\t"				// Notify uC/OS-II of ISR
	"inc	r16	\n\t"
	"sts	OSIntNesting,r16	\n\t"	
	"cpi	r16,0x01	\n\t"					//if(OSIntNesting == 1)
	"brne	not_first_int	\n\t"	
	
	"in	r28,__SP_L__	\n\t"					// Z = SP
	"in	r29,__SP_H__	\n\t"	
	"lds	r30,OSTCBCur	\n\t"					// Z = OSTCBCur->OSTCBStkPtr
	"lds	r31,OSTCBCur+1	\n\t"
	"st	z+,r28	\n\t"						// Save Y pointer
	"st	z+,r29	\n\t"
	
	"not_first_int:	\n\t"
	"ldi	r16,256 - (16000000 / 100 / 1024)	\n\t"
												// Reload timer to overflow at a rate of OS_TICKS_PER_SEC
												// at a prescaler of 1024 and CPU_CLOCK_HZ AVR clock
												//256 - (CPU_CLK_FREQ / OS_TICKS_PER_SEC / 1024)
	"out	0x26,r16	\n\t"					//reload TCNT0
	
	"sei	\n\t"							// Enable interrupts
	
	"call	OSTimeTick	\n\t"					// Call uC/OS-IIs tick updating function
	"call	OSIntExit	\n\t"					// Notify uC/OS-II about end of ISR
	
	POP_SREG_ISR()						// Restore all registers and status register
	POP_ALL()
	
	"reti	\n\t"
	::);
}


