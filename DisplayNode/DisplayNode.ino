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
const long CAN_SPEED = 500E3; // Defines the speed of the CAN network communication. Matching this speed across all nodes is crucial for reliable communication.
const uint16_t DISTANCE_MAX_RAW = 2000; //Constants that define the expected range of distance values. These are used to filter / format the displayed data.
const uint16_t DISTANCE_MIN_RAW = 20;

// Globals
LiquidCrystal_I2C g_lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
bool g_canActive = false;
long g_tofSensorId = 0x10;
long g_tofSensorId2 = 0x20;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  g_lcd.init();
  g_lcd.backlight();
  g_lcd.clear();
  g_lcd.print("CAN Receiver");

  // Initialize CAN bus
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
  Read CAN messages and interpreting their contents. This involves:
    - Waiting for a packet to arrive on the CAN bus.
    - Verifying the packet's ID to ensure it matches the expected value for relevant data.
    - Extracting the data from the packet and converting it into a meaningful format, like a distance measurement
*/
void readCANBus() {
  byte bytearray[2] = {0}; 
  uint16_t distance = 0;
  int pktSize;
  int pktId;

  // Reset LCD
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
      
  // Try to parse packet
  pktSize = CAN.parsePacket();
  pktId = CAN.packetId();

  if ((pktId == g_tofSensorId || pktId == g_tofSensorId2) && !CAN.packetRtr() && pktSize > 0){      
    // Read packet data if available
    for (int byteIndex = 0; CAN.available() && byteIndex < pktSize; ++byteIndex) {
      bytearray[byteIndex % 2] = CAN.read(); // Modulo operation ensures we don't exceed the bytearray bounds
      
      if (byteIndex % 2 == 1){ // We have a pair of bytes to process
        distance = (bytearray[1] << 8) | bytearray[0]; // Combine two bytes into one integer
        displayDistance(distance, pktId);
        printPacket(pktId);
      }
    }
  } else if (pktId != g_tofSensorId || pktId != g_tofSensorId2) {
      Serial.println("Error: CAN packet ID is unknown");
      Serial.print("Unknown Packet ID: ");
      Serial.println(pktId);
      g_lcd.print("Unknown Pkt id");
      g_lcd.setCursor(0, 1);
      g_lcd.print(pktId);
  } else if (pktSize == 0) {
      Serial.println("Error: empty packet");
      g_lcd.print("Err: empty pkt");
  }else {    
      Serial.println("Waiting for packet");
      g_lcd.print("Waiting for pkt");
  }
}

/*
  Display distance value and "Out of Range" is value are out of range.
*/
void displayDistance(uint16_t distance, int pktId) {
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
  
  if (distance < DISTANCE_MIN_RAW || distance > DISTANCE_MAX_RAW) {
    g_lcd.print("Out of Range");
    g_lcd.setCursor(0, 1);
    g_lcd.print("Pkt id: ");
    g_lcd.print(pktId);
  } else {
    g_lcd.print("Dist: ");
    g_lcd.print(distance);
    g_lcd.print(" mm"); 
    g_lcd.setCursor(0, 1);
    g_lcd.print("Pkt id: ");
    g_lcd.print(pktId);
  }
}

void printPacket(int pktId){
  Serial.println(pktId.parsePacket);
}
