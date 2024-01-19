#include "OV7670.h"
#include "WiFiCredentials.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "BMP.h"
#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true

const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 17;
const int D2 = 16;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

// DEFINE NETWORK
const char* ssid = MY_SSID;
const char* password = MY_PASSWORD;
const char* serverName = "http://coskos.pythonanywhere.com";
const char* serverPath = "/postFrame";

// GLOBAL OBJECTS (camera, frame_header, http_client)
OV7670* camera;
unsigned char bmpHeader[BMP::headerSize];
HTTPClient client;

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.printf("Initializing Camera ...");
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  Serial.printf(" Done.\nInitializing BMP ...");
  BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);
  Serial.printf(" Done.\n");

  // Begin the HTTP client connection here
  while (!client.begin(String(serverName) + serverPath)) {
    delay(10);
  }
  Serial.printf("HTTP Client Connection Established.\n");
}

void loop() {
  Serial.printf("Gathering Frame Data ...");
  camera->oneFrame();
  Serial.printf(" Done.\n");

  sendCamPhoto();
  delay(20);  // Adjust delay as needed
}

void sendCamPhoto() {
  // Add Header Fields
  client.addHeader("Host", serverName);
  client.addHeader("Content-Type", "multipart/form-data; boundary=ESP32CAMERAFEED");
  const char* head = "\n--ESP32CAMERAFEED\r\nContent-Disposition: form-data; name=\"file\"; filename=\"/frame.bmp\"\r\nContent-Type: image/bmp\r\n\r\n";
  const char* tail = "\r\n--ESP32CAMERAFEED--\r\n";
  uint32_t imageLen = camera->frameBytes + 66;
  uint32_t extraLen = strlen(head) + strlen(tail);
  uint32_t totalLen = imageLen + extraLen;
  char s_totalLen[256];
  itoa(totalLen, s_totalLen, 10);
  client.addHeader("Content-Length", s_totalLen);

  // Initialize POST request space in memory
  uint8_t* body = (uint8_t*)malloc(sizeof(uint8_t) * totalLen);
  assert(body != NULL);
  int offset = 0;

  // Write MIME Head String
  int end = offset + strlen(head);
  for (int i = offset; i < end; i++) {
    body[i] = head[i - offset];
  }
  offset = end;

  // Write MIME Body (BMP Image)
  // Write BMP Header
  end = offset + 66;
  int j = 0;
  for (int i = offset; i < end; i++) {
    body[i] = bmpHeader[j++];
  }
  offset = end;
  // Write BMP Body
  end = offset + camera->frameBytes;
  j = 0;
  for (int i = offset; i < end; i++) {
    body[i] = camera->frame[j++];
  }
  offset = end;

  // Write MIME Tail String
  end = offset + strlen(tail);
  j = 0;
  for (int i = offset; i < end; i++) {
    body[i] = tail[j++];
  }
  offset = end;

  // Send POST Request & Get Response Code
  int code = client.POST(body, offset);
  assert(offset == totalLen);
  free(body);

  // Print HTTP Code and Server Response
  Serial.printf("HTTP Code: %d (%s)\nServer Response: %s\n", code, client.errorToString(code).c_str(), client.getString().c_str());
}