#include <FlexCAN_T4.h>

FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can0;
// Modifed Hezheng Code for Watchdog Node


// by Hezheng May 2024, tm1638 module working with CAN
// upload to both device, do not change wiring
// use the modified TM1638 library for Teensy 4.1 in this file


#define TIMEOUT 1000 // timeout at 500ms (50 * 10)
#define ITERATION_TIME 50 // check for timeout every 50ms
#define NODE_COUNT 2 // todo: fill in the amt of nodes you have

#define RELAY_PIN 5

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
  bool readyToDrive = true;
  while (!readyToDrive) { }

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
}

enum NODE {
    // todo: populate this with the nodes you want
    pedalBox = 0x011F,
    IMD = 0x665281F,
};

// global to keep track of what timed out
NODE timed_out_node;

// todo: fill out with nodes
uint16_t pedalBoxTimer = 0;
uint16_t IMD_Timer = 0;

// todo: fill out this array
uint16_t *timerArray[] = {&pedalBoxTimer, &IMD_Timer}; // array to keep track of timers
NODE nodeArray[] = {pedalBox, IMD}; // Array to keep track of each node


void shut_off_car() {
    digitalWrite(RELAY_PIN, LOW);
    // TODO: define a way to recover or just let it hang till reset
    while (1) Serial.println(timed_out_node);
}

void resetTimer(const CAN_message_t &msg) {
    enum NODE id = msg.id;

    switch (id) {
        case (pedalBox):
            pedalBoxTimer = 0;
            break;
        case (IMD):
            IMD_Timer = 0;
            break;
        default:
            break;
    }

    Serial.println(id);
}

void loop() {
    Can0.events();
     
    long long start_time = millis();

    for (uint8_t i = 0; i < NODE_COUNT ; i++) {
        (*timerArray[i])++;

        if ((*timerArray[i]) > TIMEOUT) {
            timed_out_node = nodeArray[i]; // set timedout flag to proper id
            shut_off_car();
        }
            
    }

    while (millis() - start_time < ITERATION_TIME) {}  // stall until iteratin time is hit
}
