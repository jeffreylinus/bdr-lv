#include <FlexCAN_T4.h>
#include "TM1638.h"
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can0;

// Modifed Hezheng Code for PedalBox Node

#define DRIVE_ENABLE_ID 0x0C1F
#define DRIVE_ENABLE_ON 0x01

#define SET_CURRENT_ID 0x011F
#define SET_CURRENT_LEN 2 // TODO: figure out len

void setup(void) {
  Serial.begin(115200); delay(400);
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.mailboxStatus();
}



int readPot() {
    // todo: average pot readings


}

int mapResistanceToVal(int resistance ) {
    // todo: map resistance to a value between -3247 and 3247

}



void loop() {
    CAN_message_t msg;
    msg.id = SET_CURRENT_ID;

    // todo: figure this out

    int payload[SET_CURRENT_LEN] = mapResistanceToVal(readPot());
    msg.len = SET_CURRENT_LEN; 

    for (uint8_t j = 0; j < msg.len; j++ ) msg.buf[j] = payload[j]; // set the payload to the length
    Can0.write(msg);
}
