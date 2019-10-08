#include <wiringPi.h>
#include <thread>
#include <chrono>

public class Timing() {
private:
void (*cb)(void);
std::thread manualThread; 
std::thread gpioThread;
bool shouldRun = false;
bool shouldShutdown = false;
// Wether it was last on or off
bool gpioLastState = false;
// GPIO pin
int pinNumber;
// Frame time
auto frameTime = std::chrono::milliseconds(16.66667);
// GPIO delay time (3/10 of a millisecond)
auto gpioDelayTime = std::chrono::microseconds(3);


void callbackFunc() {
  // This runs 60 times a second
  if (shouldRun) {
    // shouldrun can be off to pause, etc
    cb();
  }
}



void manualCb() {
while (!shouldShutdown) {
// Continue to run this
// Wait for a 60th of a second
auto start = std::chrono::steady_clock::now();
auto diff = now - start;
    auto end = now + frameTime;
callbackFunc();
// Sleep until a frame has passed
std::this_thread::sleep_until(end);
}
}

void gpioCb() {
while (!shouldShutdown) {
int readResult = digitalRead(pinNumber);
if (!gpioLastState && readResult) {
// Pin is on right now and it was off last
gpioLastState = true;
// Run callback function
callbackFunc();
} else {
if (!readResult) {
// It is now off
gpioLastState = false;
}
}
// Wait for a very short time to not overwhelm the gpio
std::this_thread::sleep_for(gpioDelayTime)
}
}



public:
  Timing() {}
  
  void setupVGAMode(int pinNum) {
    // Setup GPIO with physical mapping
    pinNumber = pinNum;
    wiringPiSetupGpio();
    // Only read pin
    pinMode(pinNum, INPUT);
    // Setup thread
    gpioThread = std::thread(gpioCb);
    // Starts automatically
  }
  }
  
  void start() {
  shouldRun = true;
  }
  
  void stop() {
  shouldRun = false;
  }
  
  void shutdown() {
  shouldShutdown = true;
  }
  
  void setupManualMode() {
    manualThread = std::thread(manualCb);
    // Starts automatically
  }
  
  void setCallback(void (*callback)(void)) {
    cb = callback;
  }
}
