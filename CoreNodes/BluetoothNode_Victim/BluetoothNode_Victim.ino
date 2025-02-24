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
    if (c == '\n' || c == '\r') {  //Process only when newline detected
      g_input.trim();  //Remove extra spaces and newline characters
      Serial.print("Processing Bluetooth Command: ");
      Serial.println(g_input);  // Debug print
      processBluetoothInput(g_input);
      g_input = "";  //Clear buffer after processing
    } else {
      g_input += c;  //Append characters until newline
    }
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
  Serial.println("Sending BT status");
  if (btStatus == 1){
    btStatus = 20;
    Serial.println("Bluetooth status is paired");
  } else{
    btStatus = 10;
    Serial.println("Bluetooth status is unpaired");
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
    input.trim();  // Remove whitespace
    input.replace("\r", "");  // Remove Carriage Return
    input.replace("\n", "");  // Remove Newline

    Serial.print("Received RAW Input: ");
    for (size_t i = 0; i < input.length(); i++) {
        Serial.print("0x");
        Serial.print(input[i], HEX);
        Serial.print(" ");
    }
    Serial.println();  // Newline for readability
    
    if (input.indexOf("LED_ON") != -1) {  
        digitalWrite(13, HIGH);
        BT.println("LED ON");
        Serial.println("LED turned ON!");
    } else if (input.indexOf("LED_OFF") != -1) {  
        digitalWrite(13, LOW);
        BT.println("LED OFF");
        Serial.println("LED turned OFF!");
    } else if (input.indexOf("HELP") != -1) {  
        BT.println("Commands: LED_ON, LED_OFF");
    } else {  
        BT.print("Unknown Command: ");
        BT.println(input);
    }
}

