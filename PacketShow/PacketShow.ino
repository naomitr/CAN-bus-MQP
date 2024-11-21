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
  Read CAN messages and interpret their contents.
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

  if ((pktId == g_tofSensorId || pktId == g_tofSensorId2) && !CAN.packetRtr() && pktSize > 0) {
    // Display packet ID
    Serial.print("Packet ID: 0x");
    Serial.println(pktId, HEX);

    // Display entire packet in binary format
    Serial.print("Packet Content (Binary): ");
    while (CAN.available()) {
      byte dataByte = CAN.read();
      printByteInBinary(dataByte); // Print byte in binary format
      Serial.print(" ");
    }     
    Serial.println();

    // Process and display distance
    distance = (bytearray[1] << 8) | bytearray[0];
    displayDistance(distance, pktId);

  } else if (pktId != g_tofSensorId && pktId != g_tofSensorId2) {
    Serial.println("Error: CAN packet ID is unknown");
    g_lcd.print("Unknown Pkt id");
    g_lcd.setCursor(0, 1);
    g_lcd.print(pktId, HEX);
  } else if (pktSize == 0) {
    Serial.println("Error: empty packet");
    g_lcd.print("Err: empty pkt");
  } else {
    Serial.println("Waiting for packet");
    g_lcd.print("Waiting for pkt");
  }
}

/*
  Display distance value and "Out of Range" if the value is out of range.
*/
void displayDistance(uint16_t distance, int pktId) {
  g_lcd.clear();
  g_lcd.setCursor(0, 0);

  if (distance < DISTANCE_MIN_RAW || distance > DISTANCE_MAX_RAW) {
    g_lcd.print("Out of Range");
    g_lcd.setCursor(0, 1);
    g_lcd.print("Pkt id: ");
    g_lcd.print(pktId, HEX);
  } else {
    g_lcd.print("Dist: ");
    g_lcd.print(distance);
    g_lcd.print(" mm");
    g_lcd.setCursor(0, 1);
    g_lcd.print("Pkt id: ");
    g_lcd.print(pktId, HEX);
  }
}

/*
  Print a single byte in binary format to the Serial Monitor.
*/
void printByteInBinary(byte dataByte) {
  for (int i = 7; i >= 0; --i) { // Read bits from MSB to LSB
    Serial.print(bitRead(dataByte, i));
  }
}
