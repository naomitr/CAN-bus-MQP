#include <SoftwareSerial.h>

// Define software serial pins for Bluetooth HC-06
SoftwareSerial BT(10, 11);

void setup() {
    Serial.begin(9600);  // Serial monitor
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
    }
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
