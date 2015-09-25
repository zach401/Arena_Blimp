/*
 * main.c
 */

#include "msp430g2553.h"

// AIR
#include "hal_board.h"
#include "cc11xL_spi.h"
#include "hal_int_rf_msp_exp430g2.h"
#include "cc11xL_easy_link_msp_exp_430g2_reg_config.h"
#include "stdlib.h"

/******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0
#define PKTLEN              12
/******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void registerConfig(void);
static void runTX(char left, char right, char up, char LED);
static void createPacket(uint8 txBuffer[], char left, char right, char up, char LED);
static void radioRxTxISR(void);
/******************************************************************************
 *
 */

#define RIGHTCHANNEL INCH_3;
#define LEFTCHANNEL INCH_4;
#define RIGHTPIN BIT3;
#define LEFTPIN BIT4;
#define UPBIT BIT5;
#define DOWNBIT BIT4;

void configureMSP430();
void configureAIR();
void configureADC();
char readRightJoystick();
char readLeftJoystick();
int readADC(char channel);
char range(char channel, int input);
char readUpDown();
char readUp();
char readDown();
char readLED(unsigned char prev);

const int LEFTRANGE[] = {287, 372, 457, 561, 669, 770};
const int RIGHTRANGE[] = {241, 347, 458, 572, 668, 750};

volatile unsigned char LEDFLAG;

void main(void)
{
	unsigned char RIGHTCTRL = 3;
 	unsigned char LEFTCTRL = 3;
 	unsigned char UPCTRL = 0;
 	unsigned char LEDCTRL = 0;
 	volatile unsigned char state = 1;

 	configureMSP430();
 	configureAIR();
 	configureADC();
 	while(1)
 	{
		switch(state)
		{
			case 1:
			LEFTCTRL = readLeftJoystick();
			state = 2;
			break;

			case 2:
			RIGHTCTRL = readRightJoystick();
			state = 3;
			break;

			case 3:
			UPCTRL = readUpDown();
			state = 4;
			break;

			case 4:
			LEDCTRL = readLED(LEDCTRL);
			state = 5;
			break;

			case 5:
			runTX(LEFTCTRL, RIGHTCTRL, UPCTRL, LEDCTRL);
			state = 1;
			break;
		}
 	}
}

void configureMSP430()
{
	  WDTCTL = WDTPW + WDTHOLD;                    // Stop WDT
	  BCSCTL1 = CALBC1_1MHZ;                       // Set range
	  DCOCTL = CALDCO_1MHZ;

	  P2DIR &= ~(BIT5 + BIT4 + BIT3);					// P2.5 and P2.4 inputs
	  P2OUT |=BIT5 + BIT4 + BIT3;					  	// pull-UP resistor rather than pull-DOWN resistor
	  P2REN |= BIT5 + BIT4 + BIT3;                          // enable pull-up resistor for switch
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

void configureADC()
{
	  ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  		// Vcc as ref, ADC10ON, interrupt enabled
	  ADC10CTL1 = RIGHTCHANNEL;                          			// right channel
	  ADC10AE0 |= RIGHTPIN;                            				// right pin ADC option select
	  P1DIR &= ~LEFTPIN;
	  P1DIR &= ~RIGHTPIN;
	  P1SEL |= LEFTPIN;												// set left pin to be used by ADC
	  P1SEL |= RIGHTPIN;											// set right pin to be used by ADC
}

char readRightJoystick()
{
	int temp = readADC(1);
	return range(1, temp);
}

char readLeftJoystick()
{
	int temp = readADC(0);
	return range(0, temp);
}

// channel = 0 for left, channel = 1 for right
int readADC(char channel)
{
	ADC10CTL0 &= ~ENC;
	if (channel)
	{
		ADC10CTL1 = RIGHTCHANNEL;                          			// right channel
		ADC10AE0 |= RIGHTPIN;                            			// right pin ADC option select
	}
	else
	{
		ADC10CTL1 = LEFTCHANNEL;                          			// right channel
	    ADC10AE0 |= LEFTPIN;                            			// right pin ADC option select
	}
	ADC10CTL0 |= ENC + ADC10SC;            							// Sampling and conversion start
	LPM0;															// wait until ADC is finished
	return ADC10MEM;
}

// channel = 0 for left, channel = 1 for right
char range(char channel, int data)
{
	if (channel)				// right channel
	{
		if (data <= RIGHTRANGE[0])
			return 0;
		else if (data > RIGHTRANGE[0] && data <= RIGHTRANGE[1])
			return 1;
		else if (data > RIGHTRANGE[1] && data <= RIGHTRANGE[2])
			return 2;
		else if (data > RIGHTRANGE[2] && data <= RIGHTRANGE[3])
			return 3;
		else if (data > RIGHTRANGE[3] && data <= RIGHTRANGE[4])
			return 4;
		else if (data > RIGHTRANGE[4] && data <= RIGHTRANGE[5])
			return 5;
		else if (data > RIGHTRANGE[5])
			return 6;
	}
	else
	{
		if (data <= LEFTRANGE[0])
			return 0;
		else if (data > LEFTRANGE[0] && data <= LEFTRANGE[1])
			return 1;
		else if (data > LEFTRANGE[1] && data <= LEFTRANGE[2])
			return 2;
		else if (data > LEFTRANGE[2] && data <= LEFTRANGE[3])
			return 3;
		else if (data > LEFTRANGE[3] && data <= LEFTRANGE[4])
			return 4;
		else if (data > LEFTRANGE[4] && data <= LEFTRANGE[5])
			return 5;
		else if (data > LEFTRANGE[5])
			return 6;
	}
}

char readUpDown()
{
	char up = readUp();
	char down = readDown();

	if ((up && down) || (!up && !down))
		return 0;
	else if (up && !down)
		return 1;
	else if (down && !up)
		return 2;
}

char readUp()
{
	if (P2IN & BIT5)
		return 0;
	else
		return 1;
}

char readDown()
{
	if (P2IN & BIT4)
			return 0;
		else
			return 1;
}

char readLED(unsigned char prev)
{
	if ((LEDFLAG == 1) && !(P2IN &= BIT3))
	{
		LEDFLAG = 0;
		if (prev == 0)
			return 1;
		else if (prev == 1)
			return 0;
	}

	else if ((LEDFLAG == 0) && (P2IN &= BIT3))
	{
		LEDFLAG = 1;
		if (prev == 0)
			return 0;
		else if (prev == 1)
			return 1;
	}
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	__low_power_mode_off_on_exit();
}

static void runTX(char c1, char c2, char c3, char c4)
{
  // Initialize packet buffer of size PKTLEN + 1
  uint8 txBuffer[PKTLEN+1] = {0};

   P2SEL &= ~0x40; // P2SEL bit 6 (GDO0) set to one as default. Set to zero (I/O)
  // connect ISR function to GPIO0, interrupt on falling edge
  trxIsrConnect(GPIO_0, FALLING_EDGE, &radioRxTxISR);

  // enable interrupt from GPIO_0
  trxEnableInt(GPIO_0);

	// create a random packet with PKTLEN + 2 byte packet counter + n x random bytes
	createPacket(txBuffer, c1, c2, c3, c4);

  // write packet to tx fifo
  cc11xLSpiWriteTxFifo(txBuffer,sizeof(txBuffer));

  // strobe TX to send packet
  trxSpiCmdStrobe(CC110L_STX);

	// wait for interrupt that packet has been sent.
	// (Assumes the GPIO connected to the radioRxTxISR function is set
	// to GPIOx_CFG = 0x06)
	while(!packetSemaphore);

	// clear semaphore flag
	packetSemaphore = ISR_IDLE;

  halLedToggle(LED1);
   __delay_cycles(250000);
   halLedToggle(LED1);
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
#ifdef PA_TABLE
  uint8 paTable[] = PA_TABLE;
#endif

  // reset radio
  trxSpiCmdStrobe(CC110L_SRES);
  // write registers to radio
  for(i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
    writeByte =  preferredSettings[i].data;
    cc11xLSpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
  }
#ifdef PA_TABLE
  // write PA_TABLE
  cc11xLSpiWriteReg(CC11xL_PA_TABLE0,paTable, sizeof(paTable));
#endif
}

static void createPacket(uint8 txBuffer[], char c1, char c2, char c3, char c4)
{
  txBuffer[0] = PKTLEN;                     // Length byte
  txBuffer[1] = c1;
  txBuffer[2] = c2;
  txBuffer[3] = c3;
  txBuffer[4] = c4;
}
