/*****************************************
  Authors: Naomi Treto & Sarah Meaney
  Date: 3/3/2025

  Description:
  This program functions as a Bluetooth-controlled CAN bus sender. It receives 
  commands via a Bluetooth module (HC-06) and transmits the received data over 
  the CAN bus. The system continuously listens for Bluetooth input and 
  ensures CAN connectivity is maintained.

  Features:
  - Bluetooth communication for remote CAN message transmission.
  - Dynamic CAN message construction from received Bluetooth data.
  - Automatic CAN bus initialization and reinitialization upon failure.
  - Support for variable-length CAN messages (up to 8 bytes).
  - Serial debugging output for monitoring incoming Bluetooth data and CAN transmission.

  License:
  This program is licensed under the GNU General Public License v3.0.

*****************************************/


#include <SoftwareSerial.h>
#include <CAN.h>

// Define software serial pins for Bluetooth HC-06
SoftwareSerial BT(10, 11);

// Constants
const long CAN_SPEED = 500E3;

// Globals
bool g_canActive    = false;
long g_canID        = 0x40;

void setup() {
    Serial.begin(9600);  // Serial monitor

    initCAN();

    BT.begin(9600);  // Default HC-06 baud rate
    Serial.println("Bluetooth module ready. Waiting for input...");
}

void loop() {
    if (BT.available()) {  // Check if data is coming from Bluetooth
        String receivedData = "";
        while (BT.available()) {
            char c = BT.read();
            receivedData += c;
            delay(5);  // Small delay to ensure complete message reception
        }
        Serial.print("Received via Bluetooth: ");
        Serial.println(receivedData);

      if (g_canActive){
        sendDataOverCAN(receivedData);
      }
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

void sendDataOverCAN(String receivedData) {
  byte canData[2] = {0};
  bool rtr        = false;
  int dlc         = sizeof (canData);


}

/***
#include <SoftwareSerial.h>
#include <CAN.h>

SoftwareSerial BT(10, 11); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND
String g_inputString = "";


void setup()  
{
  // set digital pin to control as an output
  pinMode(13, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Send test message to other device
  BT.println("Hello from Arduino");
}

void loop() 
{
  if (BT.available()){
  // if text arrived in from BT serial...
    char incomingChar = BT.read();

    while (incomingChar != '\r' && incomingChar != '\n'){
      g_inputString += incomingChar;
      Serial.println(g_inputString);
    }
    Serial.println(g_inputString);
    processCommand(g_inputString);
s
    a=(BT.read());
    if (a=='1')
    {
      digitalWrite(13, HIGH);
      BT.print("Received: ");
      BT.println(a,HEX);
      BT.println("LED on");
    }
    else if (a=='2')
    {
      digitalWrite(13, LOW);
      BT.print("Received: ");
      BT.println(a,HEX);
      BT.println("LED off");
    }
    else if (a=='?')
    {
      BT.println("Send '1' to turn LED on");
      BT.println("Send '2' to turn LED off");
    }
    else {
      BT.print("Received: ");
      BT.println(a);
      BT.println("Unknown input");
    }
  }
}

void processCommand(String command){
  BT.print("Received: ");
  BT.println(command);

  if (command == "LED_ON"){
    digitalWrite(13, HIGH);
    BT.println("LED turned ON");
  } else if (command == "LED_OFF"){
    digitalWrite(13, LOW);
    BT.println("LED Turned OFF");
  } else if (command = "?") {
    BT.println("Send 'LED_ON' to turn LED on");
    BT.println("Send 'LED_OFF' to turn LED off");
  } else {
    BT.println("Unknown command");
  }
}

***/

#include <SoftwareSerial.h>
#include <CAN.h>

// Define software serial pins for Bluetooth HC-06
SoftwareSerial BT(10, 11);

// Constants
const long CAN_SPEED = 500E3;

// Globals
bool g_canActive = false;
long g_canID     = 0x40; // Set an appropriate CAN ID for Bluetooth messages

void setup() {
    Serial.begin(9600);  // Serial monitor
    BT.begin(9600);  // Default HC-06 baud rate

    initCAN();

    Serial.println("Bluetooth module ready. Waiting for input...");
}

void loop() {
    if (BT.available()) {  // Check if data is coming from Bluetooth
        String receivedData = "";
        while (BT.available()) {
            char c = BT.read();
            receivedData += c;
            delay(5);  // Small delay to ensure complete message reception
        }
        
        Serial.print("Received via Bluetooth: ");
        Serial.println(receivedData);

        if (g_canActive) {
            sendDataOverCAN(receivedData);
        }
    }
}

/*
  Initialize CAN bus
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
  Convert received Bluetooth data into a format suitable for CAN bus
*/
void sendDataOverCAN(String receivedData) {
    // Ensure the message fits within CAN's 8-byte limit
    byte canData[8] = {0};
    int dlc = min(receivedData.length(), 8); // Limit to 8 bytes

    // Copy received data into byte array
    for (int i = 0; i < dlc; i++) {
        canData[i] = receivedData[i]; // Convert char to byte
    }

    bool rtr = false; // Regular message, not a Remote Transmission Request

    // Begin CAN packet
    if (!CAN.beginPacket(g_canID, dlc, rtr)) {
        Serial.println("CAN begin failed");
        g_canActive = false;
    }
    else if (!CAN.write(canData, dlc)) {      
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

