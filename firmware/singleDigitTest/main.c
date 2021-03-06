#include <msp430.h>
#include <stdint.h>

// Segment pin defines
#define SEG_A   BIT0
#define SEG_B   BIT1
#define SEG_C   BIT2
#define SEG_D   BIT3
#define SEG_E   BIT4
#define SEG_F   BIT5
#define SEG_G   BIT6
#define SEG_DP  BIT7
#define ALLSEG  SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G + SEG_DP

// Numeral segment defines
#define ZERO    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F
#define ONE     SEG_B + SEG_C
#define TWO     SEG_A + SEG_B + SEG_G + SEG_E + SEG_D
#define THREE   SEG_A + SEG_B + SEG_C + SEG_D + SEG_G
#define FOUR    SEG_B + SEG_C + SEG_G + SEG_F
#define FIVE    SEG_A + SEG_F + SEG_G + SEG_C + SEG_D
#define SIX     SEG_A + SEG_F + SEG_G + SEG_C + SEG_D + SEG_E
#define SEVEN   SEG_A + SEG_B + SEG_C
#define EIGHT   SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G
#define NINE    SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G
#define HEX_A   SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G
#define HEX_B   SEG_C + SEG_D + SEG_E + SEG_F + SEG_G
#define HEX_C   SEG_A + SEG_D + SEG_E + SEG_F
#define HEX_D   SEG_B + SEG_C + SEG_D + SEG_E + SEG_G
#define HEX_E   SEG_A + SEG_D + SEG_E + SEG_F + SEG_G
#define HEX_F   SEG_A + SEG_E + SEG_F + SEG_G

void WDT_waitAsec(void);                        // use watchdog timer as 1 second interval, sleep during this time
uint8_t dispSegments(uint8_t value);

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	                // stop watchdog timer

	SFRRPCR |= SYSRSTRE | SYSRSTUP;             // Enable internal pullup resistor on reset pin

	P1OUT |= ALLSEG;                            // preload segment pins high (high = off)
	P1DIR |= ALLSEG;                            // segment pins are output pins

	P2OUT |= BIT0 + BIT1 + BIT6 + BIT7;         // preload segment pins high
	P2DIR |= BIT0 + BIT1 + BIT6 + BIT7;         // set as output, makes ULP adviser happy
	PAOUT |= 0xFF;
	PADIR |= 0xFF;

	PM5CTL0 &= ~LOCKLPM5;                       // Disable the GPIO power-on default high-impedance mode to activate
                                                // previously configured port settings

	__enable_interrupt();                       // Enable global interrupts

	for(;;)                                     // loop forever!
	{
	    for(uint8_t i=0; i<16; i++){
            P1OUT |= ALLSEG;                        // disable all segments first
            P1OUT &= ~dispSegments(i);              // pull segments low
            WDT_waitAsec();
	    }
	}
}

void WDT_waitAsec()
{
    // WDT as 1s interval counter
    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_4;
    SFRIE1 |= WDTIE;
    __bis_SR_register(LPM3_bits);               // Enter LPM3
}

// WDT interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
#else
#error Compiler not supported!
#endif
{
        WDTCTL = WDTPW + WDTHOLD;               // Stop WDT
        __bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3
}

uint8_t dispSegments(uint8_t value) {
    switch (value) {
    case(0):
        return ZERO;
    case(1):
        return ONE;
    case(2):
        return TWO;
    case(3):
        return THREE;
    case(4):
        return FOUR;
    case(5):
        return FIVE;
    case(6):
        return SIX;
    case(7):
        return SEVEN;
    case(8):
        return EIGHT;
    case(9):
        return NINE;
    case(10):
        return HEX_A;
    case(11):
        return HEX_B;
    case(12):
        return HEX_C;
    case(13):
        return HEX_D;
    case(14):
        return HEX_E;
    case(15):
        return HEX_F;
    default:
        return ZERO;

    } // end switch
} // end dispSegments()
