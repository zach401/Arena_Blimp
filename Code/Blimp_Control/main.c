/*/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/csl/CSL.h>

// AIR
/*****************************************************************************
* INCLUDES
*/
#include "msp430.h"
#include "hal_board.h"
#include "cc11xL_spi.h"
#include "hal_int_rf_msp_exp430g2.h"
#include "cc11xL_easy_link_msp_exp_430g2_reg_config.h"
#include "stdlib.h"
/******************************************************************************
 * CONSTANTS
 */

/******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0

#define PKTLEN              30
/******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void registerConfig(void);
static void runRX(void);
static void radioRxTxISR(void);


#define RIGHTMOTOR0 BIT0
#define RIGHTMOTOR1 BIT1
#define LEFTMOTOR0 BIT2
#define LEFTMOTOR1 BIT3
#define UPMOTOR0 BIT4
#define UPMOTOR1 BIT5
#define LEDPIN BIT4

void configureMSP430();
void configureAIR();
void configureTimer();

void updateRight(volatile char index);
void updateLeft(char index);
void updateUP(char state);
void updateLED(char state);


unsigned int RightFwRs = RIGHTMOTOR0;
unsigned int LeftFwRs = LEFTMOTOR0;
unsigned int UpFwRs = UPMOTOR0;
unsigned int Speed[] = {238, 190, 150, 0, 150, 190, 238};

volatile char RIGHTCTRL = 3;
char LEFTCTRL = 3;
char UPCTRL;
char LEDCTRL;

volatile unsigned char LEDON;

/*
* main.c
*/
void main(void)
{
      volatile unsigned int toSend;
      volatile unsigned char LEDCTRL;
      volatile unsigned char state;

      configureAIR();
      configureMSP430();

		while(1)
		{
			runRX();
		}
}

void configureMSP430()
{
	 WDTCTL = WDTPW + WDTHOLD;                    // Stop WDT
	 BCSCTL1 = CALBC1_1MHZ;                       // Set range
	 DCOCTL = CALDCO_1MHZ;

	 CSL_init();                    			 // Activate Grace-generated configuration
	 TA0CCR1 = 0;
	 TA1CCR1 = 0;
	 P1DIR |= LEDPIN;
	 P1OUT &= ~LEDPIN;
	 P2DIR |= UPMOTOR0 + UPMOTOR1 + RIGHTMOTOR0 + RIGHTMOTOR1 + LEFTMOTOR0 + LEFTMOTOR1;
	 P2OUT &= ~UPMOTOR0;
}

void configureAIR()
{
	//init MCU
	  halInitMCU();
	  //init LEDs
	  halLedInit();
	  //init button
	  halButtonInit();
	  halButtonInterruptEnable();
	  // init spi
	  exp430RfSpiInit();
	  // write radio registers
	  registerConfig();
}

void updateRight(volatile char index)
{
	if (index <= 6)
	{
		if (index < 3)					// establish forward or reverse
			{
				RightFwRs = RIGHTMOTOR0;
				P2OUT &= ~(RIGHTMOTOR1+RIGHTMOTOR0);
			}
		else
		{
			RightFwRs = RIGHTMOTOR1;
			P2OUT &= ~(RIGHTMOTOR1+RIGHTMOTOR0);
		}
		TA1CCR1 = Speed[index];			// change PWM duty cycle
	}
}

void updateLeft(char index)
{
	if (index <= 6)
	{
		if (index < 3)					// establish forward or reverse
				{
					LeftFwRs = LEFTMOTOR0;
					P2OUT &= ~(LEFTMOTOR1 + LEFTMOTOR0);
				}

		else
		{
			LeftFwRs = LEFTMOTOR1;
			P2OUT &= ~(LEFTMOTOR0 + LEFTMOTOR1);
		}
		TA0CCR1 = Speed[index];			// change PWM duty cycle
	}
}

void updateUP(char state)
{
	if (state == 1)
	{
		P2OUT |= UPMOTOR0;
		P2OUT &= ~UPMOTOR1;
	}
	else if (state == 0)
	{
		P2OUT &= ~UPMOTOR0;
		P2OUT &= ~UPMOTOR1;
	}
	else if (state == 2)
	{
		P2OUT |= UPMOTOR1;
		P2OUT &= ~UPMOTOR0;
	}
}

void updateLED(char state)
{
	if (state == 1)
		P1OUT |= LEDPIN;
	else if (state == 0)
		P1OUT &= ~LEDPIN;

}

void RightTimerA1_0 ()
{
	if (RIGHTCTRL != 3)
		P2OUT |= RightFwRs;
}

void RightTimerA1_1 ()
{
	P2OUT &= ~RightFwRs;
}

void LeftTimerA0_0 ()
{
	if (LEFTCTRL != 3)
		P2OUT |= LeftFwRs;
}

void LeftTimerA0_1 ()
{
	P2OUT &= ~LeftFwRs;
}

static void runRX(void)
{
  uint8 rxBuffer[12] = {0};
  uint8 rxBytes;
  uint8 rxBytesVerify;
  char dataRX = 0;

   P2SEL &= ~0x40; // P2SEL bit 6 (GDO0) set to one as default. Set to zero (I/O)
  // connect ISR function to GPIO0, interrupt on falling edge
  trxIsrConnect(GPIO_0, FALLING_EDGE, &radioRxTxISR);

  // enable interrupt from GPIO_0
  trxEnableInt(GPIO_0);


  // set radio in RX
  trxSpiCmdStrobe(CC110L_SRX);

  // reset packet counter
  packetCounter = 0;

  // infinite loop
  while(!dataRX)
  {
    // wait for packet received interrupt
    if(packetSemaphore == ISR_ACTION_REQUIRED)
    {
        cc11xLSpiReadReg(CC110L_RXBYTES,&rxBytesVerify,1);

        do
        {
          rxBytes = rxBytesVerify;
          cc11xLSpiReadReg(CC110L_RXBYTES,&rxBytesVerify,1);
        }
        while(rxBytes != rxBytesVerify);

        cc11xLSpiReadRxFifo(rxBuffer,(rxBytes));

        // check CRC ok (CRC_OK: bit7 in second status byte)
        if(rxBuffer[rxBytes-1] & 0x80)
        {
          // toggle led
          halLedToggle(LED1);
          // update packet counter

          LEFTCTRL = rxBuffer[1];
          RIGHTCTRL = rxBuffer[2];
          UPCTRL = rxBuffer[3];
          LEDCTRL = rxBuffer[4];
          dataRX = 1;

          updateLeft(LEFTCTRL);
          updateRight(RIGHTCTRL);
          updateUP(UPCTRL);
          updateLED(LEDCTRL);
        }

      // reset packet semaphore
      packetSemaphore = ISR_IDLE;

      // set radio back in RX
      trxSpiCmdStrobe(CC110L_SRX);

    }
  }
}

static void radioRxTxISR(void) {

  // set packet semaphore
  packetSemaphore = ISR_ACTION_REQUIRED;
  // clear isr flag
  trxClearIntFlag(GPIO_0);
}

static void registerConfig(void) {
  uint8 writeByte;
  uint16 i;
  // reset radio
  trxSpiCmdStrobe(CC110L_SRES);
  // write registers to radio

  for(i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++)
  {
    writeByte =  preferredSettings[i].data;
    cc11xLSpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
  }
}

