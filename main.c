#include <msp430.h> 

#define audioInput INCH_0
#define POT1 INCH_3
#define POT2 INCH_1
#define PWM BIT6
#define Down BIT5
#define GND BIT1

unsigned int audio =100;
unsigned int freq=200;
unsigned int plasmaStrength = 100;
unsigned int pot1value = 100;
unsigned int pot2value = 100;
unsigned int audiolevel = 100;



unsigned int analogRead(unsigned int pin);

int i =0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    P1SEL |= PWM;    //p1.2 in TA mode PWM
    P1DIR |= PWM +GND;    //p1.2 output
    P1OUT &=~GND;

    TACCTL1 = OUTMOD_3 + CCIE;
    TACCR0 = 1000;
    TACCR1 = 500;
    TACTL = TASSEL_2 + MC_1 + ID_0;
    __enable_interrupt();

       while(1)
       {				pot1value = analogRead(POT1);
       	   	   	   	   pot2value = analogRead(POT2);
       	   	   	   	   audiolevel = analogRead(audioInput);
    	   	   	   	   plasmaStrength = (pot2value >> 4);
		        	   audio = (audiolevel >> 1) + plasmaStrength - 200;// Read the analog input from channel 5
		        	   freq = ((pot1value>> 2) + 400);
		        	   if(freq - audio <70)
		        	   {
		        		   audio = freq -70;
		        		   TACTL |= TACLR;
		        		   _delay_cycles(20);
		        		   TACTL &= ~TACLR;

		        	   }

       }

}

unsigned int analogRead(unsigned int pin) {
 ADC10CTL0 = ADC10ON + ADC10SHT_0 + SREF_0;
 ADC10CTL1 = ADC10SSEL_0 + pin;
 if (pin==INCH_0){
 ADC10AE0 = 0x01;
 }
 else if (pin==INCH_1){
 ADC10AE0 = 0x02;
 }
 else if (pin==INCH_2){
 ADC10AE0 = 0x04;
 }
 else if (pin==INCH_3){
  ADC10AE0 = 0x08;
  }
 else if (pin==INCH_4){
  ADC10AE0 = 0x10;
  }
 else if(pin==INCH_5){
 ADC10AE0 = 0x20;
 }
 else if(pin==INCH_6){
  ADC10AE0 = 0x40;
  }
 else if(pin==INCH_7){
  ADC10AE0 = 0x80;
  }
 ADC10CTL0 |= ENC + ADC10SC;
 //_delay_cycles(10);
 while (1) {
 if (((ADC10CTL0 & ADC10IFG)==ADC10IFG)) {
 ADC10CTL0 &= ~(ADC10IFG +ENC);
 break;
 }
 }
 return ADC10MEM;
 }

#pragma vector=TIMER0_A1_VECTOR
__interrupt void timer_taccr1(void)
{
  switch (TA0IV)         // Efficient switch-implementation
  {
    case  TA0IV_NONE://the interrupt is disabled

      break;

    case  TA0IV_TACCR1: // Compare match interrupt (TACCR1 - duty cycle)


    	_delay_cycles(2);
        TACCR1 = audio;
        TACCR0 = freq;
      break;

    default:
      break;
  }
}
