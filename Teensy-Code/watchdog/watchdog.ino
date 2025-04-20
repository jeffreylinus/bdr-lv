#include <FlexCAN_T4.h>

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;
// Modifed Hezheng Code for Watchdog Node


// by Hezheng May 2024, tm1638 module working with CAN
// upload to both device, do not change wiring
// use the modified TM1638 library for Teensy 4.1 in this file

#define ITERATION_TIME 50 // check for timeout every 50ms
#define TIMEOUT (1000 / ITERATION_TIME) // timeout at 1000ms (50 * 20)
#define NODE_COUNT 2 // todo: fill in the amt of nodes you have

#define WATCHDOG_PIN 23
#define IMD_PIN 19

bool ready = false;

// #define MAKE_TIMER(nodeID) { .id = nodeID, .timer = 0 }

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

  pinMode(WATCHDOG_PIN, OUTPUT);
  pinMode(IMD_PIN, OUTPUT);

  digitalWrite(WATCHDOG_PIN, LOW);
  digitalWrite(IMD_PIN, LOW);




  while (!ready) Can0.events();
}

enum NODE {
    // todo: populate this with the nodes you want
    pedalBox = 0x011F,
    IMD = 0x18FF01F4,
    readyToDrive = 0x07FE
};

///* refactor if there's time*/
//typedef struct {
//    enum NODE id;
//    uint16_t watchdog;
//
//} nodeTimer;
//
//nodeTimer timers[] = {
//    MAKE_TIMER(pedalBox),
//    MAKE_TIMER(IMD)
//}
//
//
//
///* end refactor*/


// global to keep track of what timed out
NODE timed_out_node;

// todo: fill out with nodes
volatile uint16_t pedalBoxTimer = 0;
volatile uint16_t IMD_Timer = 0;

// todo: fill out this array
volatile uint16_t *timerArray[] = {&pedalBoxTimer, &IMD_Timer}; // array to keep track of timers
NODE nodeArray[] = {pedalBox, IMD}; // Array to keep track of each node


void shut_off_car(int pin) {
    digitalWrite(pin, LOW);
    // TODO: define a way to recover or just let it hang till reset
    while (1) {
      Serial.println(timed_out_node, HEX);
      delay(100);
    }
  }

void resetTimer(const CAN_message_t &msg) {
    NODE id = static_cast<NODE>(msg.id);

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
              shut_off_car(IMD_PIN);
            }
            IMD_Timer = 0;
            break;
        case(readyToDrive):
          ready = true;
          break;
        default:
            break;
    }

}

void loop() {
    Can0.events();
    digitalWrite(WATCHDOG_PIN, HIGH);
    digitalWrite(IMD_PIN, HIGH);
    long long start_time = millis();

    for (uint8_t i = 0; i < NODE_COUNT ; i++) {
        (*timerArray[i])++;

        if ((*timerArray[i]) > TIMEOUT) {
            timed_out_node = nodeArray[i]; // set timedout flag to proper id
            shut_off_car(WATCHDOG_PIN);
        }
            
    }

    while (millis() - start_time < ITERATION_TIME) Can0.events();  // stall until iteratin time is hit
}
