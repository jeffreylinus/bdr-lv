#include <FlexCAN_T4.h>

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can0;

// Modifed Hezheng Code for PedalBox Node

#define DRIVE_ENABLE_ID 0x0C1F
#define DRIVE_ENABLE_ON 0x01
#define DRIVE_ENABLE_LEN 1

#define SET_CURRENT_ID 0x011F
#define SET_CURRENT_LEN 2 // TODO: figure out len

#define CURRENT_MAX 3247
#define ANALOG_READ_MAX 4095

#define ITERATION_TIME 10 // send msg every 10 ms

const int potPin = 14;

void drive_enable();

void setup(void) {
  Serial.begin(115200); delay(400);
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.mailboxStatus();
  analogReadResolution(12); // set analog read to 12 bits of precision
  drive_enable();
}

void drive_enable(){
    CAN_message_t msg;
    msg.id = SET_CURRENT_ID;
    msg.len = DRIVE_ENABLE_LEN; 
    for (uint8_t j = 0; j < msg.len; j++ ) msg.buf[j] = DRIVE_ENABLE_ON; // set the payload to the length
    Can0.write(msg);
}

uint8_t curr_val[SET_CURRENT_LEN];

void mapResistanceToVal() {
    int mappedValue = map(analogRead(potPin), 0, ANALOG_READ_MAX, 0, CURRENT_MAX);
    Serial.println(mappedValue);
    curr_val[0] = (mappedValue >> 8) & 0xFF; // Extract the high byte
    curr_val[1] = mappedValue & 0xFF; // extract the low byte
}



void loop() {
    long long start_time = millis();

    CAN_message_t msg;
    msg.id = SET_CURRENT_ID;
    msg.len = SET_CURRENT_LEN;

    mapResistanceToVal(); 

    for (uint8_t i = 0; i < msg.len; i++ ) msg.buf[i] = curr_val[i];
    Can0.write(msg);

    while (millis() - start_time < ITERATION_TIME) {}  // stall until iteratin time is hit

}
