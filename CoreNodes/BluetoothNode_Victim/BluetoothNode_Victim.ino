#include <SoftwareSerial.h>
#include <CAN.h>

SoftwareSerial BT(10, 11); 

// Constants
const int RETRY_DELAY_MS = 2000;
const int LOOP_DELAY_MS = 1000;
const long CAN_SPEED = 500E3;
const long CAN_ID = 0x30;

// Globals
bool g_canActive = false;
String g_input; // stores incoming string from other device

void setup() {
  Serial.begin(9600);
  Serial.println("start of setup");
  
  pinMode(13, OUTPUT);
  BT.begin(9600);
  BT.println("Hello from Arduino");

  initCAN();
}

void loop() {
  if (!g_canActive) {
    Serial.println("CAN was inactive, reinitializing...");
    initCAN();
  }

  sendBluetoothStatus();

  while (BT.available()) {
    char c = BT.read();
    g_input += c; // Append character to the buffer
  }

  if (g_input.length() > 0) {
    Serial.println(g_input);
    processBluetoothInput(g_input);
    g_input = ""; // Clear the buffer after processing
  }

  delay(LOOP_DELAY_MS);
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

void sendBluetoothStatus() {
  uint8_t btStatus = BT.available();
  if (btStatus == 1){
    btStatus = 20;
  } else{
    btStatus = 10;
   }// 1 if device connected, 0 otherwise
  sendBluetoothStatusOverCAN(btStatus);
}

void sendBluetoothStatusOverCAN(uint8_t status) {
  byte canData[1] = {status};

  if (!CAN.beginPacket(CAN_ID, sizeof(canData), false)) {
    Serial.println("CAN begin failed");
    g_canActive = false;
  } else if (!CAN.write(canData, sizeof(canData))) {
    Serial.println("CAN write failed");
    g_canActive = false;
  } else if (!CAN.endPacket()) {
    Serial.println("CAN end failed");
    g_canActive = false;
  } else { 
    Serial.println("CAN packet sent successfully");
    Serial.print("Bluetooth Status Sent: ");
    Serial.println(status);
  }
}

void processBluetoothInput(String input) {
  if (input == "LED_ON") {
    digitalWrite(13, HIGH);
    BT.print("Received: ");
    BT.println(input);
    BT.println("LED on");
  } else if (input == "LED_OFF") {
    digitalWrite(13, LOW);
    BT.print("Received: ");
    BT.println(input);
    BT.println("LED off");
  } else if (input == "HELP") {
    BT.println("Send 'LED_ON' to turn LED on");
    BT.println("Send 'LED_OFF' to turn LED off");
  } else {
    BT.print("Received: ");
    BT.println(input);
    BT.println("Unknown command");
  }
}
