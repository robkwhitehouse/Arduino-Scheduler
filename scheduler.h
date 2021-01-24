/*
 * Simple RealTimeScheduler for Arduino
 * R K Whitehouse - Jan 2021
 */

#define MAX_TASKS 20

class Scheduler {

  private:
    byte index =  0;
    
    //List of tasks
    struct Task {
      void (*taskPointer)();
      boolean runnable = false;
      unsigned period;//Num microseconds between executions
      long nextRun;//time of next invokation
    } taskList[MAX_TASKS];

  public:
    unsigned add(void (*tptr)(), unsigned per) {
      if (index == MAX_TASKS) {
        Serial.println("Error: Too many tasks");
      } else {
        taskList[index].taskPointer = tptr;
        taskList[index].runnable = true;
        taskList[index++].period = per;
      }
      return index;
    }

    void begin() {
    }

    void run(unsigned long now) {
      Task* task;

      for (byte i = 0; i < index; i++) {
        task = &taskList[i];
        if ( task->runnable && now >= task->nextRun ) {
          task->nextRun = now + task->period;
          task->taskPointer();
        }
      }
    }
};
