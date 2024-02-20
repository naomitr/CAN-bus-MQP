/*****************************************
  Author: Olivier C
  
  Act as a receiving node on a CAN bus. Read CAN packets from bus. Iff the packet ID matches
  the expected ID, translate packet data into servo movement.
*****************************************/
#include <Servo.h>
#include <CAN.h>

// Constants
const int SERVO_PIN = 9; // digital pin on the Arduino board to which the servo motor is connected
const long CAN_SPEED = 500E3; // the communication speed for the CAN network. It's crucial that all devices on the network operate at the same speed for successful communication.
const uint16_t DISTANCE_MAX_RAW = 2000; // define thresholds for translating received data into servo movement
const uint16_t DISTANCE_MIN_RAW = 20;
const int LOOP_DELAY_MS = 1000;

// Globals
Servo g_servo;  
bool g_canActive = false; // Track the status of the CAN bus
long g_tofSensorId = 0x10;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for the serial port to connect

  g_servo.attach(SERVO_PIN); // Attaches the servo on pin 9 to the servo object
  testServo();
  initCAN();
}

void loop() {
  // Check if CAN bus has been activated
  if (!g_canActive) {
    initCAN();    
  }

  if (g_canActive) {
    readCANBus();
  }

  delay(LOOP_DELAY_MS);
}

// Initialize CAN bus
void initCAN() {
  if (!CAN.begin(CAN_SPEED)) {
    Serial.println("Starting CAN failed!");
    g_canActive = false;
  } else {
    Serial.println("CAN started successfully");
    g_canActive = true;
  }
}

/*
  Upon receiving a relevant CAN message, decode the message content to extract the data 
  intended for servo control. This data is then used to set the servo to a specific 
  position or perform a certain movement, translating digital information into physical 
  action.
*/
void readCANBus() {
  byte bytearray[2] = {0}; 
  uint16_t distanceRaw = 0;
  int pktSize;
  int pktId; 
    
  // Try to parse packet
  pktSize = CAN.parsePacket();
  pktId = CAN.packetId();
  
  if (pktSize > 0 && pktId == g_tofSensorId && !CAN.packetRtr()) {
    // Read packet data if available
    for (int byteIndex = 0; CAN.available() && byteIndex < pktSize; ++byteIndex) {
      bytearray[byteIndex % 2] = CAN.read(); // Modulo operation ensures we don't exceed the bytearray bounds
      distanceRaw = bytearray[0];
      
      if (byteIndex % 2 == 1) // We have a pair of bytes to process
        distanceRaw = (bytearray[1] << 8) | distanceRaw; // Combine two bytes into one integer
      
      uint16_t distanceMapped = constrain(distanceRaw, DISTANCE_MIN_RAW, DISTANCE_MAX_RAW); // Limit distance readings to sensor range
      distanceMapped = map(distanceMapped, DISTANCE_MIN_RAW, DISTANCE_MAX_RAW, 0, 180); // Map distance to servo range of motion (180 degrees)
      g_servo.write(distanceMapped); // Move servo
      
      Serial.print("Distance Mapped: ");
      Serial.println(distanceMapped);
    }
  } else if (CAN.packetId() != pktId) {
    Serial.println("Received packet from unknown node");
  } else {    
    Serial.println("Waiting for packet");
  }
}

/*
  Test the servo connection
*/
void testServo() {
  for (int pos = 0; pos <= 180; pos++) {
    g_servo.write(pos);
    delay(15);
  }
  for (int pos = 180; pos >= 0; pos--) {
    g_servo.write(pos);
    delay(15);
  }
}
