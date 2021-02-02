/*
   Sketch to explore and test Infra Red Sensor
   R. K. Whitehouse - Jan 2021                                                                         
*/
#include "TimerOne.h"
#include "scheduler.h"
#include "RingBuffer.h"

//#define IR_SENSOR_PIN 15
#define IR_LED_PIN 3
#define DIRECT_GPIO

#define CLOCK_RATE 20 //50,000 ticks per second - or one every 20 microseconds

//#define SCAN_PERIOD 50//50 microseconds between sensor scans
//#define REPORT_PERIOD 100000UL //do a report every 100 milliseconds
#define LED_PERIOD 50 //microseconds - Toggle the IR LED at 10000hz gives a full pulse at 5000hz


//const uint32_t tickPeriod = 1000000 / CLOCK_RATE; //in microseconds (default=20)

volatile unsigned long ticks = 0;
unsigned long lastTickTime, startTime, tickPeriod;


Scheduler scheduler;
//RingBuffer buff(100);

boolean firstRun = true;
boolean secondRun = false;
boolean firstLoop = true;

void tickHandler() {
  ticks++;
}

void setup() {
  Timer1.stop();
  Serial.begin(115200);
//  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(IR_LED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  
  // Add all our tasks to the scheduler
  int i = scheduler.add(&toggleLED, LED_PERIOD); //generates an LED flashing
 
  
//  scheduler.add(&sampleSensor, SCAN_PERIOD); //scan input every 50us (20Khz)
//  scheduler.add(&report, REPORT_PERIOD); //Report the measured input freq every 100ms


  Serial.println("Infra Red Tester Ver 0.1");
 //Set up the timer interrupt
  Timer1.initialize(CLOCK_RATE);
  Timer1.attachInterrupt(&tickHandler,CLOCK_RATE);
  Timer1.start();
}

unsigned long lastTick = 0;
void loop() {
  int i;
  
  if ( ticks > lastTick ) {
   //Run scheduled tasks
   scheduler.run(ticks * CLOCK_RATE);
   lastTick = ticks;
  }
  //Run anything else
}

 /*
  * Real time task definitons
  */

#ifdef STANDARD_GPIO
void toggleLED() {
  register byte state;
    
  state = digitalRead(IR_LED_PIN);
  digitalWrite(IR_LED_PIN, !state);
//  digitalWrite(LED_BUILTIN, !state);

}
#endif

#ifdef DIRECT_GPIO
void toggleLED() {
  register byte state;
  
///  state = PIND;
//  state ^= bit(3);
//  PORTD = state;

   PIND = bit(3);
  
//  PINB = bit(5);//flash the built-in LED
}
#endif

/*
 *Sample the IR sensor and check to see if it has changed state
 *If it has changed, and it is a rising edge, record the time period
 *since the previous rising edge
 */

 /*
byte previousState;
unsigned long pulseStartTime;

void sampleSensor() {
  register byte state = digitalRead(IR_SENSOR_PIN);
  if (state != previousState ) {//State change - log it
    if (state == HIGH) {//only log positive edges
    noInterrupts();
      register unsigned long timeNow = micros();
      pulsePeriod = timeNow - pulseStartTime;
      pulseStartTime = timeNow;
      buff.add(pulsePeriod);
    interrupts();
      pulsePeriod = 0;
    }
    previousState = state;
  } else pulsePeriod += SCAN_PERIOD;

  //Deal with the case of input stopped
  if (pulsePeriod > 1000000) {
    pulsePeriod = 1000000;
    buff.add(pulsePeriod);
  }
}


  void report() {

    unsigned long average_pulse_period = buff.average(10);//in microseconds
    if (average_pulse_period == 1000000) Serial.println(0); else if (average_pulse_period != 0) Serial.println(1000000 / average_pulse_period); //gives frequency in hz
  }
*/  
