#include <SoftwareSerial.h>  // Bluetooth Serial
#include <CAN.h>             // Built-in Arduino CAN library

// Define Bluetooth Module RX/TX (HC-06)
SoftwareSerial BT(10, 11);  // RX, TX

// Constants
const long CAN_SPEED = 500E3;   // Set CAN Bus Speed
const int CAN_ID = 0x30;        // Default CAN ID

// Global Variables
bool g_canActive = false;
String g_input;  // Stores incoming Bluetooth data

void setup() {
    Serial.begin(9600);
    Serial.println("Starting Arduino...");
    
    // Bluetooth Setup
    BT.begin(9600);
    BT.println("Bluetooth Ready!");

    // CAN Setup
    initCAN();
}

void loop() {
    // Restart CAN if it fails
    if (!g_canActive) {
        Serial.println("CAN was inactive, reinitializing...");
        initCAN();
    }

    // Process Bluetooth Input
    processBluetooth();

    delay(100);
}

void initCAN() {
    Serial.println("Initializing CAN...");

    if (!CAN.begin(CAN_SPEED)) {
        Serial.println("CAN initialization failed!");
        g_canActive = false;
    } else {
        Serial.println("CAN started successfully!");
        g_canActive = true;
    }
}

// Reads from Bluetooth and sends it over CAN
void processBluetooth() {
    if (BT.available()) {
        char c = BT.read();
        Serial.print("Received from Bluetooth: ");
        Serial.println(c);
    }
}



// Sends Bluetooth data as CAN message
void sendBluetoothDataOverCAN(String data) {
    uint8_t canData[8] = {0};  // Fixed 8-byte CAN frame
    data.getBytes(canData, sizeof(canData));  

    Serial.print("Sending to CAN: ");
    Serial.println(data);

    if (!CAN.beginPacket(CAN_ID, sizeof(canData), false)) {
        Serial.println("CAN.beginPacket() failed");
        g_canActive = false;
        return;
    }

    if (!CAN.write(canData, sizeof(canData))) {
        Serial.println("CAN.write() failed");
        g_canActive = false;
    }

    if (!CAN.endPacket()) {
        Serial.println("CAN.endPacket() failed");
        g_canActive = false;
    } else {
        Serial.println("CAN packet sent successfully");
    }
}
