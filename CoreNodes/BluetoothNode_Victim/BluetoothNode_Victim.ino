#include <SoftwareSerial.h> // remove the inverted commas after you copy the code to the IDE
#include <CAN.h>

SoftwareSerial BT(10, 11); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

// Constants
const int RETRY_DELAY_MS = 2000;
const int LOOP_DELAY_MS = 1000;
const long CAN_SPEED = 500E3;
const long CAN_ID = 0x30;

// Globals
bool g_canActive    = false;
char g_a; // stores incoming character from other device

void setup()  {
  Serial.begin(9600);
  Serial.println("start of setup");
  
  // set digital pin to control as an output
  pinMode(13, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Send test message to other device
  BT.println("Hello from Arduino");

  //Serial.println("About to init can");
  initCAN();

  //Serial.println("post init can");
}


void loop() {
  //Serial.println("checking if can is active");
  if (!g_canActive){
    Serial.println("can was unactive, init can");
    initCAN();
  }

  // NEED TO SEND BLUETOOTH STATUS

  // CHECK BLUETOOTH AVAILABLE
  // PROCESS BLUETOOTH INPUT


  //Serial.println("checking if bt is available");
  if (BT.available())
  // if text arrived in from BT serial...
  {
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
    else if (a == '\r' || a == '\n'){
      BT.println("\r or \n");
      return;
    }   
    else if (a != '1' || a != '2' || a != '?')
    {
      BT.print("Received: ");
      BT.println(a);
      BT.println("Unknown input");
    }

    // you can add more "if" statements with other characters to add more commands
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

void sendBluetoothStatusOverCAN(uint16_t status)  {
  byte canData[2] = {0};
  canData[0] = status & 0xFF;         //  LSB of status
  canData[1] = (status >> 8) & 0xFF;  //  MSB of status

  if (!CAN.beginPacket(CAN_ID, 2, false)) {
    Serial.println("CAN begin failed");
    g_canActive = false;
  } else if (!CAN.write(canData, 2)){
    g_canActive = false;
  } else if (!CAN.endPacket())  {
    Serial.println("CAN end failed");
    Serial.print("CAN dlc: ");
    Serial.println(dlc);
    g_canActive = false;
  } else {
    Serial.println("CAN packet sent successfully");
    Serial.print("Sent Data: [0x");
    Serial.print(canData[0], HEX);
    Serial.print(", 0x");
    Serial.print(canData[1], HEX);
    Serial.println("]");
  }

}
