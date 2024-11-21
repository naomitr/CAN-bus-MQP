#include <CAN.h>

// Constants
const int LOOP_DELAY_MS = 1000;
const long CAN_SPEED = 500E3;
const long g_canID   = 0x10;
const long EXPLOIT_DISTANCE = 1000;

// Globals
bool g_canActive = false;

void initCAN(); // 在 setup() 调用前声明 initCAN 函数
void sendDistanceOverCAN(uint16_t distance);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Distance Sender");
  initCAN(); // 初始化 CAN 总线
}

void loop() {
  if (!g_canActive) {
    initCAN(); // 如果 CAN 未激活，则重新初始化
  }

  // 假设手动输入的距离值
  uint16_t distance = EXPLOIT_DISTANCE; // 测试值
  sendDistanceOverCAN(distance);

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

void sendDistanceOverCAN(uint16_t distance) {
  byte canData[2] = {0};
  canData[0] = distance & 0xFF;        // LSB of distance
  canData[1] = (distance >> 8) & 0xFF; // MSB of distance

  if (!CAN.beginPacket(g_canID, 2, false)) {
    Serial.println("CAN begin failed");
    g_canActive = false;
  } else if (!CAN.write(canData, 2)) {
    Serial.println("CAN write failed");
    g_canActive = false;
  } else if (!CAN.endPacket()) {
    Serial.println("CAN end failed");
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