/*****************************************
  Original Author: Olivier C
  Modified by: Naomi Treto & Sarah Meaney
  Date: 3/3/2025

  Description:
  This program is based on an original CAN bus sender by Olivier C but has been 
  modified to improve debugging and enhance robustness. It retrieves distance 
  measurements from a ToF sensor and sends the data over the CAN bus.

  Major Modifications:
  - Added extensive debugging messages to track program execution and identify potential issues.
  - Improved sensor initialization with additional debug prints for better visibility of failures.
  - Refactored setup process to provide clearer feedback on the initialization sequence.
  - Enhanced CAN transmission error handling by providing detailed failure messages.
  - Fixed minor typos in Serial output for better readability.

  License:
  This program is licensed under the GNU General Public License v3.0.

*****************************************/


#include <Wire.h>
#include <VL53L0X.h>
#include <CAN.h>

// Constants
const int RETRY_DELAY_MS = 2000;
const int LOOP_DELAY_MS = 1000;
const long CAN_SPEED = 500E3; // communication speed for the CAN bus, set to 500 kbps, which is a standard speed for many CAN networks.

// Globals
VL53L0X g_tofSensor;
bool g_canActive    = false; // flag indicating whether the CAN bus is active and ready for communication.
long g_canID        = 0x10; //  identifier for CAN packets sent from this device, ensuring that messages can be correctly routed and recognized in the network.

void setup() {
  Serial.println("start of setup");
  Serial.begin(9600);
  Wire.begin();

  // Initialize CAN bus
  Serial.println("pre initCAN");
  initCAN();
  Serial.println("post initCAN");

  setupToF();
}

void loop() {
  uint16_t distance;

  // Init CAN if not already active
  if (!g_canActive) {
    initCAN();
  }
  
  distance = g_tofSensor.readRangeContinuousMillimeters();

  Serial.print("Dist: ");
  Serial.println(distance);
  
  // Check for timeout
  if (g_tofSensor.timeoutOccurred()) {
    Serial.println("sensor Timeout");
    return; // Do not proceed with sending if there's a sensor timeout.
  }
   
  if (g_canActive) { 
    sendDistanceOverCAN(distance);
  }
  delay(LOOP_DELAY_MS);
}

/*
  Init CAN
*/
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
  Setup ToF sensor
*/
void setupToF() {
  Serial.println("in setup ToF");
  g_tofSensor.setTimeout(1000);

  Serial.println("before the while loop");
  while (!g_tofSensor.init()) {
      Serial.println("Failed to detect and initialize sensor!");
      delay(RETRY_DELAY_MS);
  }
  
  Serial.println("about to start tof continuous");
  g_tofSensor.startContinuous();
}

/*
  Send sensor reading to CAN bus.  Packages the distance data into a CAN 
  message and sending it over the network. This includes formatting the 
  data correctly according to CAN protocol standards and handling any
  errors that may occur during transmission.
*/
void sendDistanceOverCAN(uint16_t distance) {
  byte canData[2] = {0};
  bool rtr        = false;
  int dlc         = sizeof (canData);
  
  canData[0] = distance & 0xFF;        // LSB of distance
  canData[1] = (distance >> 8) & 0xFF; // MSB of distance
    
  if (!CAN.beginPacket(g_canID, dlc, rtr)) {
    Serial.println("CAN begin failed");
    g_canActive = false;
  }
  else if (!CAN.write(canData, 2)){      
    Serial.println("CAN write failed");    
    g_canActive = false;
  }
  else if (!CAN.endPacket()) {      
    Serial.println("CAN end failed");
    Serial.print("CAN dlc: ");
    Serial.println(dlc);
    g_canActive = false;
  } else {
    Serial.println("CAN packet sent successfully");
  }
}
