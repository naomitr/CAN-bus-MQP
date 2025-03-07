#include <SoftwareSerial.h> // remove the inverted commas after you copy the code to the IDE
SoftwareSerial BT(10, 11); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND
void setup()  
{
  // set digital pin to control as an output
  pinMode(13, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Send test message to other device
  BT.println("Hello from Arduino");
}
char a; // stores incoming character from other device
void loop() 
{
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
