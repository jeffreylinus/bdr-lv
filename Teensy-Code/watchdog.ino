#include <FlexCAN_T4.h>
#include "TM1638.h"
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can0;

// Modifed Hezheng Code for Watchdog Node


// by Hezheng May 2024, tm1638 module working with CAN
// upload to both device, do not change wiring
// use the modified TM1638 library for Teensy 4.1 in this file


#define TIMEOUT 1000
#define ITERATION_TIME 100
#define NODE_COUNT 1 // todo: fill in the amt of nodes you have

void setup(void) {
  Serial.begin(115200); delay(400);
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(resetTimer);
  Can0.mailboxStatus();

  // todo: find some way to set ready to drive
  bool readyToDrive;
  while (!readyToDrive) { }
}

enum NODE {
    // todo: populate this with the nodes you want
    pedalBox = 0x011F,
};

// todo: fill out with nodes
uint8_t pedalBoxTimer = 0;

// todo: fill out this array
uint8_t *timerArray[] = {&pedalBoxTimer}

void shut_off_car() {
    // todo: fill in code to shut off the car
    exit(1);

}

void resetTimer(const CAN_message_t &msg) {
    enum NODE id = msg.id

    switch (id) {
        case (pedalBox):
            *pedalBoxTimer = 0;
            break;
        default:
            shut_off_car();
            break;
    }
}

void loop() {
    Can0.events();
     
    long long start_time = millis();

    for (uint8_t i = 0; i < NODE_COUNT ; i++) {
        (*timerArray[i])++;

        if ((*timerArray[i]) > TIMEOUT)
            shut_off_car();
    }


    long long end_time = millis();
    while (end_time - start_time < ITERATION_TIME)  // stall until iteratin time is hit
        end_time = millis();

}
