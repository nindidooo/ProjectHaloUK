/*
 ____  ____  _____   ____  ____  ___  ____    _   _    __    __    _____    __  __  _  _ 
(  _ \(  _ \(  _  ) (_  _)( ___)/ __)(_  _)  ( )_( )  /__\  (  )  (  _  )  (  )(  )( )/ )
 )___/ )   / )(_)( .-_)(   )__)( (__   )(     ) _ (  /(__)\  )(__  )(_)(    )(__)(  )  ( 
(__)  (_)\_)(_____)\____) (____)\___) (__)   (_) (_)(__)(__)(____)(_____)  (______)(_)\_)
                                Brought to you by Jabawoki
                                    www.jabawoki.com
                                    
On boot up, they run Mode 1 once, then default to Mode 2 below

Mode 1:           Fade from 0% to 100% to 80% in sequence from Led 1 (inside) to Led 3 (outside)
Mode 2:           Daytime operation – all Leds at 80%
Mode 3:           Night operation – all Leds at 50%
Mode 4:           Signal Mode – Outer Led & inner Led rings at 30% - middle ring off
Mode 5:           All rings at 100%

Modes 3,4 & 5 are selected by a HIGH signal on each of the 3 opto’s.

Revision History: Development Version

V1  Experimental - no ndictor function
V2  Experimental - no indicator function
V3  Production - all functions OK
V4  Experimental - adding debounce function
V5  Experimental - adding debounce function
V6  Experimental - adding debounce function
V7  Production - Added Indicator debouncing routine and 10ms timer
V8  Production - IndicatorState variable is now set correctly on startup
V9  Production - Increased Brightness Levels & added brightness key

Revision History: Production Version
V1 Production - Updated pin outs for new hardware + brightness tweeks
V2 Prodiction - Sorted pin mappings and adjusted timers for faster chip
*/
 
// Definition of interrupt names
#include < avr/io.h >
// ISR interrupt service routine
#include < avr/interrupt.h >

#define TIMER_EXPIRED  0
#define INDICATOR_DEBOUNCEDELAY  25  // 100ms debounce time; increase if required
#define INDICATOR_LOCKOUTDELAY 150  // 100 = 1s
#define TIMER_TICK  10               // 10ms timer tick interval 

volatile int led1 = 10;              // OUT1 (14) the pin that the inner ring is attached to // Timer 1
volatile int led2 = 3;               // OUT2 (1) the pin that the middle ring is attached to // Timer 2 
volatile int led3 = 6;               // OUT3 (10) the pin that the outer ring is attached to // Timer 0
volatile int led4 = 9;               // OUT4 (13) optional output for expansion // Timer 1
volatile int input1 = 17;            // IN1 (26) the input from the indicators
volatile int input2 = 16;            // IN2 (25) the input from the main beam
volatile int input3 = 15;            // IN3 (24) the input from the driving lights
volatile int input4 = 14;            // IN4 (23) optional input for expansion

// Jumpers are on pins 0 & 1 & are also serial lines if needed

int leddelay = 6;
int dayORnight = 0;
int mainBeam = 0;

/* 
Brightness Levels:
25  = 10%
50  = 20%
75  = 30%
100 = 40%
125 = 50%
150 = 60%
175 = 70%
200 = 80%
225 = 90%
255 = 100%
*/

volatile int dayLevel = 200;
volatile int signalInner = 50;
volatile int signalCentre = 50;
volatile int signalOuter = 50;
volatile int nightLevelInner = 100;
volatile int nightLevelMiddle = 100;
volatile int nightLevelOuter = 100;

int CurrentIndicatorState = 0;   // the current reading from the input pin
int LastIndicatorState = 0;      // the previous reading from the input pin
bool IndicatorState;             // debounced indicator state

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long IndicatorDebounceTimer = 0;    // Indicator noise debounce timer
long IndicatorLockoutTimer = 0;     // Indicator lockout timer
long previousMillis = 0;      // will store last timer was updated

bool resetAfterInd = true;



// the setup routine runs once on boot:
void setup()  
{ 
  // Set pin states
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(input4, INPUT);
  pinMode(input3, INPUT);
  pinMode(input2, INPUT);
  pinMode(input1, INPUT);
  
  // Set timer frequencies
  TCCR2B = TCCR2B & 0b11111000 | 0x01;
  TCCR1B = TCCR1B & 0b11111000 | 0x01;
  TCCR0B = TCCR0B & 0b11111000 | 0x01;  
} 

void msTimers(void) {
  
  unsigned long currentMillis = millis();
 
  if( currentMillis - previousMillis > TIMER_TICK ) 
  {
    // save the last timer 
    previousMillis = currentMillis;  
    
    // decrement any system timers that are running
    if ( IndicatorDebounceTimer != 0 )
      IndicatorDebounceTimer--; 
    if ( IndicatorLockoutTimer != 0 )
      IndicatorLockoutTimer--; 
  }
  
}

// Startup routine
void launch(int qq)  
{
  for ( int q=0; q<qq; q++ )
  // Led 1 Fade Sequence
  {
    for ( int p=0; p<=255; p+=1 )
    {
      analogWrite(led1, p);
      delay(leddelay);
    }
    for ( int q=255; q>=dayLevel; q-=1 )
    {
      analogWrite(led1, q);
      delay(leddelay);
    }
    // Led 2 Fade Sequence
    for ( int r=0; r<=255; r+=1 )
    {
        analogWrite(led2, r);
        delay(leddelay);
    }
    for ( int s=255; s>=dayLevel; s-=1 )
    {
        analogWrite(led2, s);
        delay(leddelay);
    }
    // Led 3 Fade Sequence
    for ( int t=0; t<=255; t+=1 )
    {
        analogWrite(led3, t);
        delay(leddelay);
    }
    for ( int u=255; u>=dayLevel; u-=1 )
    {
        analogWrite(led3, u);
        delay(leddelay);
    }
  } 

  // initialise indicator states
  CurrentIndicatorState = digitalRead(input1);
  LastIndicatorState = CurrentIndicatorState;
  if (CurrentIndicatorState == HIGH) 
  {
    IndicatorState = false;
  }
  else
  {
    IndicatorState = true;
  }
}

void DebounceIndicator(void) {
  
  // read the state of the indicator into a local variable:
  int Indicator_reading = digitalRead(input1);

  // If indicator changed, due to noise or change of state:
  if (Indicator_reading != LastIndicatorState) 
  {
    // reset the indicator debouncing timer
    IndicatorDebounceTimer = INDICATOR_DEBOUNCEDELAY;
  } 
  
  if (IndicatorDebounceTimer == TIMER_EXPIRED) 
  {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the indicator state has changed:
    if (Indicator_reading != CurrentIndicatorState) 
    {
      CurrentIndicatorState = Indicator_reading;
      // start indicator lockout timer
      IndicatorLockoutTimer = INDICATOR_LOCKOUTDELAY;
      
      // check the state of indicator is ON
      if (CurrentIndicatorState == LOW) 
      {
        // indicator state is in transition
        IndicatorState = true;
      }
    }
  }
  
  if (IndicatorLockoutTimer == TIMER_EXPIRED) 
  {
      // check the state of indicator is still OFF
      if (CurrentIndicatorState == HIGH) 
      {
        IndicatorState = false;
      }
  }
  
  // save the reading.  Next time through the loop,
  // it'll be the lastIndicatorState:
  LastIndicatorState = Indicator_reading;
}




// normal use
void Running()
{
  dayORnight = digitalRead(input3);
  mainBeam = digitalRead(input2);
  
  if (mainBeam == LOW && IndicatorState == false) 
  {
    if(resetAfterInd)
    {
      // Fade all Leds back on
      for ( int t=0; t<=255; t+=1 )
      {
        analogWrite(led1, t);
        analogWrite(led2, t);
        analogWrite(led3, t);
        delay(leddelay);
      }
      for ( int u=255; u>=dayLevel; u-=1 )
      {
        analogWrite(led1, u);
        analogWrite(led2, u);
        analogWrite(led3, u);
        delay(leddelay);
      }
    }
    else
    {
      /* once ramped up enter here */
      digitalWrite(led1,HIGH);
      digitalWrite(led2,HIGH);
      digitalWrite(led3,HIGH);
    }
  }
  else if (dayORnight == HIGH  && IndicatorState == false) 
  {
      analogWrite(led1, dayLevel);
      analogWrite(led2, dayLevel);
      analogWrite(led3, dayLevel);
  } 
  else if (IndicatorState == false)
  {
      analogWrite(led1, nightLevelInner);
      analogWrite(led2, nightLevelMiddle);
      analogWrite(led3, nightLevelOuter);
  }
  else if (IndicatorState == true)
  {
      // here is where you want to set 
      // the polarity of led1 and led2 to be reversed
      analogWrite(led1, 0);
      analogWrite(led2, 0);
      analogWrite(led3, 0);
      resetAfterInd = true;
  }

  // else if (IndicatorState == true)
  // {
  //     analogWrite(led1, signalInner);
  //     analogWrite(led2, signalCentre);
  //     analogWrite(led3, signalOuter);
  // }
        

}


// the loop routine runs over and over again forever:
void loop()  
{ 
  launch(1);
  while(1) {
    msTimers();
    DebounceIndicator();
    Running();
  }
}

