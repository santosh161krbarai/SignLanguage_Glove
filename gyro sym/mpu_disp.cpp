#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <MPU6050.h>
#include <FS.h>
#include <LittleFS.h>

MPU6050 mpu;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "ESP32-MPU";
const char* password = "12345678";

int16_t ax, ay, az;
int16_t gx, gy, gz;
float pitch, roll, yaw = 0;
unsigned long lastTime = 0;

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.println("Client connected");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin();

  mpu.initialize();
  if (!mpu.testConnection()) Serial.println("MPU not connected!");
  else Serial.println("MPU connected successfully.");

  WiFi.softAP(ssid, password);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  if (!LittleFS.begin(true)) { // auto-format if needed
    Serial.println("LittleFS Mount Failed");
    return;
  }

  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  lastTime = millis();
}

void loop() {
  webSocket.loop();
  server.handleClient();

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  mpu.getAcceleration(&ax, &ay, &az);
  float fax = (float)ax / 16384.0;
  float fay = (float)ay / 16384.0;
  float faz = (float)az / 16384.0;
// Serial.printf("fax: %.2f, fay: %.2f, faz: %.2f\n", fax, fay, faz);
  pitch = atan2(fax, sqrt(fay * fay + faz * faz)) * 180.0 / PI;
  roll  = atan2(fay, sqrt(fax * fax + faz * faz)) * 180.0 / PI;
  // roll  = fmod(roll + 360.0, 360.0);
  // roll  = roll > 340 ? 340 : roll;
  // roll = roll < 5 ? 5 : roll;
  // pitch = fmod(pitch + 360.0, 360.0);
  // pitch = pitch < 5 ? 5 : pitch;
  // pitch = pitch > 340 ? 340 : pitch;
  
  mpu.getRotation(&gx, &gy, &gz);
  float fgz = (float)gz / 131.0;
  yaw += fgz * dt; // correct integration
  // yaw = yaw < 5 ? 5 : yaw;
  // yaw = yaw > 340 ? 340 : yaw;
  // if (yaw < 0) yaw += 360;

   Serial.printf("Pitch: %.2f, Roll: %.2f, Yaw: %.2f\n", pitch, roll, yaw);

  String data = String(pitch) + "," + String(roll) + "," + String(yaw);
  webSocket.broadcastTXT(data);

  delay(20);
}
