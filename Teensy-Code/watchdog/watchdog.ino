#include <FlexCAN_T4.h>

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;
// Modifed Hezheng Code for Watchdog Node


// by Hezheng May 2024, tm1638 module working with CAN
// upload to both device, do not change wiring
// use the modified TM1638 library for Teensy 4.1 in this file


#define TIMEOUT 500 // timeout at 500ms (50 * 10)
#define ITERATION_TIME 50 // check for timeout every 50ms
#define NODE_COUNT 2 // todo: fill in the amt of nodes you have

#define RELAY_PIN 3

void setup(void) {
  Serial.begin(115200); delay(400);
  //Can0.setTX(1); // Set CAN3 TX to pin 0
  //Can0.setRX(0); // Set CAN3 RX to pin 1


  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();

  Can0.enableFIFOInterrupt();
  Can0.onReceive(resetTimer);
  Can0.mailboxStatus();

  // Set mailbox filters to accept extended IDs
  //Can0.setMBFilter(REJECT_ALL); // Reject all IDs by default
  //Can0.setMBFilter(FIFO, EXT);  // Accept all extended IDs in FIFO


  // todo: find some way to set ready to drive
  bool readyToDrive = true;
  while (!readyToDrive) { }

  pinMode(RELAY_PIN, OUTPUT);
}

enum NODE {
    // todo: populate this with the nodes you want
    pedalBox = 0x011F,
    IMD = 0x18FF01F4,
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
    while (1) {
      Serial.println(timed_out_node, HEX);
      delay(100);
    }
  }

void resetTimer(const CAN_message_t &msg) {
    enum NODE id = msg.id;
    uint16_t resistance = 0;

    switch (id) {
        case (pedalBox):
            pedalBoxTimer = 0;
            break;
        case (IMD):
            resistance = msg.buf[0];
            resistance <<= 8;
            resistance |= msg.buf[1];
            if (resistance < 200){
              shut_off_car();
            }
            IMD_Timer = 0;
            break;
        default:
            break;
    }

}

void loop() {
    Can0.events();
    digitalWrite(RELAY_PIN, HIGH);
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
