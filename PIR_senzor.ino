#include<string.h>
#include<avr/wdt.h> //for watchdog timer
//for I^2C comunication
#include<LiquidCrystal_I2C.h>
#include<Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define rgb_R PC1
#define rgb_G PC2
#define rgb_B PC3
#define buzzPIN PD3
#define outPIR PC0
#define size 4

#define L1 PD4
#define L2 PD5
#define L3 PD6
#define L4 PD7
#define C1 PB0
#define C2 PB1
#define C3 PB2
#define C4 PB3

//password setup
char password[size+1] = {'2','A','5','B'};
char keep_pass[size+1];
short int i;

int old_read;

//for reading the rising front of each key
int old_1;
int old_2;
int old_3;
int old_A;
int old_4;
int old_5;
int old_6;
int old_B;
int old_7;
int old_8;
int old_9;
int old_C;
int old_asterix;
int old_0;
int old_htg;
int old_D;

void adc_init();
uint16_t read_adc(uint8_t channel);
void PIR_alarm();
int Password();
void ReadKey();
void rgb_red();
void rgb_green();
void watchdogSetup();

void setup()
{
  watchdogSetup();
  //ADC Noise Reduction mode
  SMCR|=(1<<SE) | (1<<SM0);
  SMCR&=~((1<<SM1) | (1<<SM2));
  
  PRR|=(1<<PRTIM1) | (1<<PRTIM2) | (1<<PRSPI); //Power Reduction Register -- stop timers (1,2) clk -- stop SPI clk
  
  DDRD|=(1<<buzzPIN) | (1<<L1) | (1<<L2) | (1<<L3) | (1<<L4);
  DDRC&=~((1<<outPIR) | (1<<C1) | (1<<C2) | (1<<C3) | (1<<C4));
  DDRC|=(1<<rgb_R) | (1<<rgb_G) | (1<<rgb_B);
  adc_init();
  rgb_green();
  Serial.begin(9600);

  //delay(10000); //timp calibrare senzor PIR
  lcd.init();
  lcd.backlight();
}
  
void loop()
{
  PIR_alarm();
  delay(500); 
}


void adc_init()//adc initialization
{
 ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));//set division factor between system clock frequency and the input clock to the ADC- 128
 ADMUX |= (1<<REFS0);//AVcc with external capacitor at Aref pin 
 ADCSRA |= (1<<ADEN);//enable ADC
 ADCSRA |= (1<<ADSC);//ADC start conversion
}

uint16_t read_adc(uint8_t channel)
{
 ADMUX &= 0xFF;//set input AO to A8
 ADMUX |= channel;//select chanel AO to A8
 ADCSRA |= (1<<ADSC);//start conversion
 if(!(ADCSRA & (1<<ADSC)));//wait while adc conversion are not updated 
 return ADCW;//read and return voltage
}

void PIR_alarm()
{
  wdt_reset();
  int read=read_adc(0);
  Serial.println(read);
  if(read && (!old_read))
  {
    rgb_red();
    lcd.setCursor(0,0);
    lcd.print("Parola:");
    PORTD|=(1<<buzzPIN); 
    wdt_reset();
    
    while(!Password());
    PORTD&=~(1<<buzzPIN);
    rgb_green(); delay(300);
    lcd.setCursor(0,1);
    lcd.print("PAROLA CORECTA!");
    wdt_reset();
    delay(2000);
    lcd.clear();
  }
  old_read=read;
}

int Password()
{
  ReadKey();
  lcd.setCursor(8,0);
  lcd.print(keep_pass[0]);
  lcd.print(keep_pass[1]);
  lcd.print(keep_pass[2]);
  lcd.print(keep_pass[3]);

  short int j;

  if(!(strcmp(keep_pass, password)))
  {
    i=0;
    //reset password
    for(j=0; j<size; j++)
    keep_pass[j]='-';
    return 1;
  }
  else if(i==4)
  {
  lcd.setCursor(0,1);
  lcd.print("PAROLA GRESITA!");
  //reset password
  for(j=0; j<size; j++)
    keep_pass[j]='-';
  }

  return 0;
}

void ReadKey()
{
  if(i==4)
  i=0;
  
  PORTD|=(1<<L1);

  if(PINB&(1<<C1) && !old_1)
  keep_pass[i++]='1';
  else if(PINB&(1<<C2) && !old_2)
  keep_pass[i++]='2';
  else if(PINB&(1<<C3) && !old_3)
  keep_pass[i++]='3';
  else if(PINB&(1<<C4) && !old_A)
  keep_pass[i++]='A';

  old_1=PINB&(1<<C1);
  old_2=PINB&(1<<C2);
  old_3=PINB&(1<<C3);
  old_A=PINB&(1<<C4);

  PORTD&=~(1<<L1);
  PORTD|=(1<<L2);

  if(PINB&(1<<C1) && !old_4)
  keep_pass[i++]='4';
  else if(PINB&(1<<C2) && !old_5)
  keep_pass[i++]='5';
  else if(PINB&(1<<C3) && !old_6)
  keep_pass[i++]='6';
  else if(PINB&(1<<C4) && !old_B)
  keep_pass[i++]='B';

  old_4=PINB&(1<<C1);
  old_5=PINB&(1<<C2);
  old_6=PINB&(1<<C3);
  old_B=PINB&(1<<C4);

  PORTD&=~(1<<L2);
  PORTD|=(1<<L3);

  if(PINB&(1<<C1) && !old_7)
  keep_pass[i++]='7';
  else if(PINB&(1<<C2) && !old_8)
  keep_pass[i++]='8';
  else if(PINB&(1<<C3) && !old_9)
  keep_pass[i++]='9';
  else if(PINB&(1<<C4) && !old_C)
  keep_pass[i++]='C';

  old_7=PINB&(1<<C1);
  old_8=PINB&(1<<C2);
  old_9=PINB&(1<<C3);
  old_C=PINB&(1<<C4);

  PORTD&=~(1<<L3);
  PORTD|=(1<<L4);

  if(PINB&(1<<C1) && !old_asterix)
  keep_pass[i++]='*';
  else if(PINB&(1<<C2) && !old_0)
  keep_pass[i++]='0';
  else if(PINB&(1<<C3) && !old_htg)
  keep_pass[i++]='#';
  else if(PINB&(1<<C4) && !old_D)
  keep_pass[i++]='D';

  old_asterix=PINB&(1<<C1);
  old_0=PINB&(1<<C2);
  old_htg=PINB&(1<<C3);
  old_D=PINB&(1<<C4);

  PORTD&=~(1<<L4);
}

void rgb_green()
{
  //(0,255,0) green
  PORTC=(1<<rgb_G);
  PORTC&=~(1<<rgb_R) | (1<<rgb_B);
}

void rgb_red()
{
  //(255,0,0) red 
  PORTC|=(1<<rgb_R);
  PORTC&=~(1<<rgb_G) | (1<<rgb_B);
}

void watchdogSetup()
{
 cli();// disable all interrupts
 wdt_reset(); // reset the WDT timer
 /*
 WDTCSR configuration:
 WDIE = 1 : Interrupt Enable
 WDE = 1 :Reset Enable
 For 8000ms Time-out:
 WDP3 = 0 
 WDP2 = 1 
 WDP1 = 1 
 WDP0 = 1 
 */
 // Enter Watchdog Configuration mode:
 WDTCSR |= (1<<WDCE) | (1<<WDE); 
 // Set Watchdog settings:
 WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
 sei();
}

ISR(WDT_vect)// Watchdog timer interrupt
{
 lcd.clear();
}
