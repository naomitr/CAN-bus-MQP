/*****************************************
  Author: Olivier C

  Act as a receiving node on a CAN bus. Display the content of the CAN packet if the packet ID matches
  the expected ID.
*****************************************/
#include <CAN.h>
#include <LiquidCrystal_I2C.h>

// Constants
const int LCD_I2C_ADDRESS = 0x27;
const int LCD_COLUMNS = 16;
const int LCD_ROWS = 2;
const int LOOP_DELAY_MS = 1000;
const long CAN_SPEED = 500E3;
const uint16_t DISTANCE_MAX_RAW = 2000;
const uint16_t DISTANCE_MIN_RAW = 20;
const int g_outputPin = 11;  // Define a valid pin

// Globals
LiquidCrystal_I2C g_lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
bool g_canActive = false;
long g_tofSensorId = 0x10;
long g_tofSensorId2 = 0x20;
long g_bleId1 = 0x30;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(g_outputPin, OUTPUT);

  g_lcd.init();
  g_lcd.backlight();
  g_lcd.clear();
  g_lcd.print("CAN Receiver");

  // Initialize CAN bus
  initCAN();  
}

void loop() {
  int outputOfD11 = digitalRead(g_outputPin);

  if (!g_canActive) {    
    Serial.println("Reattempting CAN init...");
    CAN.end();  // Ensure CAN resets before re-init
    delay(100);
    initCAN();
  }

  if (g_canActive) {
    readCANBus();
  }

  delay(LOOP_DELAY_MS);
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
  Read CAN messages and process
*/
void readCANBus() {
  byte bytearray[2] = {0}; 
  uint16_t distance = 0;
  uint16_t info = 0;
  int pktSize;
  int pktId;

  // Ensure packet is available before reading packet ID
  pktSize = CAN.parsePacket();
  if (pktSize) {  
    pktId = CAN.packetId();

    if ((pktId == g_tofSensorId || pktId == g_tofSensorId2) && !CAN.packetRtr() && pktSize > 0) {      
      for (int byteIndex = 0; CAN.available() && byteIndex < pktSize; ++byteIndex) {
        bytearray[byteIndex % 2] = CAN.read();
        if (byteIndex % 2 == 1) { 
          distance = (bytearray[1] << 8) | bytearray[0]; 
          displayDistance(distance, pktId);
        }
      }
    } else if (pktId == g_bleId1) {
      Serial.println("Received a Bluetooth packet!");
      Serial.print("Packet ID: ");
      Serial.println(pktId, HEX);
      Serial.print("Packet Size: ");
      Serial.println(pktSize);

      uint16_t info = 0; // Reset before reading
      
      for (int byteIndex = 0; CAN.available() && byteIndex < pktSize; ++byteIndex) {
        byte receivedByte = CAN.read();
        Serial.print("Byte ");
        Serial.print(byteIndex);
        Serial.print(": ");
        Serial.println(receivedByte, HEX);

        if (pktSize == 1) { 
          info = receivedByte;  //  Handle single-byte packets correctly
        } else if (byteIndex % 2 == 1) { 
          info = (bytearray[1] << 8) | bytearray[0];  // Combine two bytes
        }
      }

      Serial.print("Final Bluetooth Data: ");
      Serial.println(info);

      displayBluetooth(info, pktId);
    } 
    // ✅ Correct CAN ID error check
    else if (pktId != g_tofSensorId && pktId != g_tofSensorId2 && pktId != g_bleId1) {
      Serial.println("Error: CAN packet ID is unknown");
      Serial.print("Unknown Packet ID: ");
      Serial.println(pktId);
      g_lcd.setCursor(0, 0);
      g_lcd.print("Unknown Pkt id");
      g_lcd.setCursor(0, 1);
      g_lcd.print(pktId);
    }
  }
}

/*
  Display distance value
*/
void displayDistance(uint16_t distance, int pktId) {
  g_lcd.clear(); //Clear the screen

  g_lcd.setCursor(0, 0);
  g_lcd.print("                "); // ✅ Fix: Avoid LCD flickering
  g_lcd.setCursor(0, 0);
  
  if (distance < DISTANCE_MIN_RAW || distance > DISTANCE_MAX_RAW) {
    g_lcd.print("Out of Range");
  } else {
    g_lcd.print("Dist: ");
    g_lcd.print(distance);
    g_lcd.print(" mm"); 
  }
  g_lcd.setCursor(0, 1);
  g_lcd.print("Pkt id: ");
  g_lcd.print(pktId);
}

/*
  Display Bluetooth status based on received values
*/
void displayBluetooth(uint16_t info, int pktId) {
  g_lcd.clear(); //Clear the screen
  
  g_lcd.setCursor(0, 0);
  
  if (info == 20) {
    g_lcd.print("Bluetooth:Paired");
  } else if (info == 10) {
    g_lcd.print("Bluetooth: RTP");
  } else {
    g_lcd.print("BT Data: ");
    g_lcd.print(info);
  }

  g_lcd.setCursor(0, 1);
  g_lcd.print("Pkt id: ");
  g_lcd.print(pktId);
}
