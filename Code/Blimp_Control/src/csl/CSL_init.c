/*
 *  ======== CSL_init.c ========
 *  DO NOT MODIFY THIS FILE - CHANGES WILL BE OVERWRITTEN
 */
 
/* external peripheral initialization functions */
extern void GPIO_init(void);
extern void BCSplus_init(void);
extern void Timer0_A3_init(void);
extern void Timer1_A3_init(void);
extern void System_init(void);
extern void WDTplus_init(void);

#include <msp430.h>

/* verify that the MSP430 headers included support the code that's generated */
#if defined(__TI_COMPILER_VERSION__)
  /* pragma required to suppress TI warning that #warning is unrecognized */
  #pragma diag_suppress 11
#endif
#if __MSP430_HEADER_VERSION__ < 1062
  #if defined(__TI_COMPILER_VERSION__)
    #warn The MSP430 headers included may be incompatible with the generated source files.  If the value of __MSP430_HEADER_VERSION__, declared by msp430.h, is less than 1062, please update your version of the msp430 headers.
  #elif defined(__GNUC__) || defined(__IAR_SYSTEMS_ICC__)
    #warning The MSP430 headers included may be incompatible with the generated source files.  If the value of __MSP430_HEADER_VERSION__, declared by msp430.h, is less than 1062, please update your version of the msp430 headers.
  #else
    /* if we can't just warn, resort to ANSI C's #error */
    #error The MSP430 headers included may be incompatible with the generated source files.  If the value of __MSP430_HEADER_VERSION__, declared by msp430.h, is less than 1062, please update your version of the msp430 headers.
  #endif
#endif
#if defined(__TI_COMPILER_VERSION__)
  /* pragma required to restore TI warnings about unrecognized directives */
  #pragma diag_default 11
#endif

/*
 *  ======== CSL_init =========
 *  Initialize all configured CSL peripherals
 */
void CSL_init(void)
{
    /* Stop watchdog timer from timing out during initial start-up. */
    WDTCTL = WDTPW + WDTHOLD;

    /* initialize Config for the MSP430 GPIO */
    GPIO_init();

    /* initialize Config for the MSP430 2xx family clock systems (BCS) */
    BCSplus_init();

    /* initialize Config for the MSP430 A3 Timer0 */
    Timer0_A3_init();

    /* initialize Config for the MSP430 A3 Timer0 */
    Timer1_A3_init();

    /* initialize Config for the MSP430 System Registers */
    System_init();

    /* initialize Config for the MSP430 WDT+ */
    WDTplus_init();

}

/*
 *  ======== Interrupt Function Definitions ========
 */

/* Interrupt Function Prototypes */
extern void LeftTimerA0_0(void);
extern void LeftTimerA0_1(void);
extern void RightTimerA1_0(void);
extern void RightTimerA1_1(void);





/*
 *  ======== Timer0_A3 Interrupt Service Routine ======== 
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR_HOOK(void)
{

	/* Capture Compare Register 0 ISR Hook Function Name */
	LeftTimerA0_0();

	/* No change in operating mode on exit */
}

/*
 *  ======== Timer0_A3 Interrupt Service Routine ======== 
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR_HOOK(void)
{

    switch (__even_in_range(TA0IV, TA0IV_TAIFG))    // Efficient switch-implementation
    {
        case TA0IV_TACCR1:
            /* Capture Compare Register 1 ISR Hook Function Name */
            LeftTimerA0_1();

            /* No change in operating mode on exit */
            break;
        case TA0IV_TACCR2:
            break;
        case TA0IV_TAIFG:
            break;
    }
}

/*
 *  ======== Timer1_A3 Interrupt Service Routine ======== 
 */
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR_HOOK(void)
{

	/* Capture Compare Register 0 ISR Hook Function Name */
	RightTimerA1_0();

	/* No change in operating mode on exit */
}

/*
 *  ======== Timer1_A3 Interrupt Service Routine ======== 
 */
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR_HOOK(void)
{

    switch (__even_in_range(TA1IV, TA1IV_TAIFG))    // Efficient switch-implementation
    {
        case TA1IV_TACCR1:
            /* Capture Compare Register 1 ISR Hook Function Name */
            RightTimerA1_1();

            /* No change in operating mode on exit */
            break;
        case TA1IV_TACCR2:
            break;
        case TA1IV_TAIFG:
            break;
    }
}

