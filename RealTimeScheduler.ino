

/*
   Sketch to demonstrate the use of my simple Arduino Scheduler class
   It has 3 timer/scheduler driven functions
   1) To flash an IR LED at the given interval
   2) To detect the IR flashes using a sensor
   3) To report the received frequency of the flashes via the serial port
   
   R. K. Whitehouse - Jan 2021                                                                         
*/

//Using Paul Stoffregen's TimerOne library from here - https://github.com/PaulStoffregen/TimerOne
//But any interrupt driven timer should work

#include "TimerOne.h"

//These two files are part of this scheduler package.
#include "scheduler.h"
#include "RingBuffer.h"

//Local pin definitions. 

#define IR_SENSOR_PIN 15
#define IR_LED_PIN 3

/*Timer tick period - remember that this is a trade-off - 
 *you need to be comfortable that you can get all of 
 *your real-time tasks done well within this period 
 */
#define TICK_PERIOD 20 //20 microseconds = 50KHz clock - this could probably go a little faster if your tasks are very simple.

//Real time task periodicities
#define SCAN_PERIOD 50 //50 microseconds between sensor scans
#define REPORT_PERIOD 100000 //do a report every 100 milliseconds
#define LED_PERIOD 100 //microseconds - Toggling the IR LED at 10000hz gives a full pulse at 5000hz


volatile unsigned long ticks = 0;
unsigned long lastTickTime;


Scheduler scheduler;
RingBuffer buff(100);

void tickHandler() {
  ticks++;
  lastTickTime = micros();
  scheduler.run(lastTickTime);
}

void setup() {

  Serial.begin(115200);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(IR_LED_PIN, OUTPUT);

  //Set up the timer interrupt
  Timer1.initialize(TICK_PERIOD);
  Timer1.attachInterrupt(&tickHandler);
 
  // Add our tasks to the scheduler
  scheduler.add(&toggleLED, LED_PERIOD); //generates an LED flashing
  scheduler.add(&sampleSensor, SCAN_PERIOD); //scan input every 50us (20Khz)
  scheduler.add(&report, REPORT_PERIOD); //Report the measured input freq every 100ms

  delay(500);
  Serial.println("Infra Red Tester Ver 0.1");

}

void loop() {
  //put anything in here that is not time critical
}

 /*
  * Real time task definitons
  */

  void toggleLED() {
    register byte state = digitalRead(IR_LED_PIN);
    digitalWrite(IR_LED_PIN, !state);

  }

/*
 *Sample the IR sensor and check to see if it has changed state
 *If it has changed, and it is a rising edge, record the time period
 *since the previous rising edge
 */


byte previousState;
unsigned long pulseStartTime, pulsePeriod;

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
  
