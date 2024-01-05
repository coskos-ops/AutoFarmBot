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

const int VSYNC = 34; //34
const int HREF = 35; //35

const int XCLK = 32; //32
const int PCLK = 33; //33

const int D0 = 27; //27
const int D1 = 17; //17
const int D2 = 16; //16
const int D3 = 15; //15
const int D4 = 14; //14
const int D5 = 13; //13
const int D6 = 12; //12
const int D7 = 4; //4

//DEFINE NETWORK
const char* ssid = (const char*) MY_SSID;
const char* password = (const char*) MY_PASSWORD;
String serverName = "http://coskos.pythonanywhere.com";
//String serverName = "http://192.168.2.85";
String serverPath = "/postFrame";

//GLOBAL OBJECTS (camera, frame_header, http_client)
OV7670 *camera;
unsigned char bmpHeader[BMP::headerSize];
HTTPClient client;

String sendPhoto() {
  String getAll;
  String getBody;

  File fb = SPIFFS.open("/small.bmp");
  if(!fb) {
    Serial.println("Failed to open image file!");
  }

  Serial.println("Connecting to server: " + serverName);
  if (client.begin(serverName + serverPath)) {
    Serial.println("Connection successful!");

    client.addHeader("Host", serverName);
    client.addHeader("Content-Type", "multipart/form-data; boundary=ESP32CAMERAFEED");
    String head = "\n--ESP32CAMERAFEED\r\nContent-Disposition: form-data; name=\"file\"; filename=\"/small.bmp\"\r\nContent-Type: image/bmp\r\n\r\n";
    String tail = "\r\n--ESP32CAMERAFEED--\r\n";
    uint32_t imageLen = fb.size();
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
    char s_totalLen[256];
    itoa(totalLen, s_totalLen, 10);
    client.addHeader("Content-Length", s_totalLen);

    uint8_t* body = (uint8_t*)malloc(sizeof(uint8_t)*totalLen);
    assert(body != NULL);
    int offset = 0;
    //Write MIME Header (ASCII)
    int end = offset + head.length();
    for (int i = offset; i < end; i++) {
      body[i] = head[i];
    }
    offset = end;
    //Serial.println("Head Written");
    //Write MIME Body (Bytes)
    int result = fb.read(body + offset, 1024);
    while(fb.available() || (offset < (fb.size() + head.length()))){
      offset += result;
      result = fb.read(body + offset, 1024);
    }  
    //Serial.println("Body Written");
    //Write MIME Tail (ASCII)
    end = offset + tail.length();
    int j = 0;
    for (int i = offset; i < end; i++) {
      body[i] = tail[j++];
    }
    offset = end;
    //Serial.println("Tail Written");
    // for(int i = 0; i < offset; i++) {
    //   Serial.printf("%c", body[i]);
    // }
    client.POST(body, offset);

    Serial.printf("Head Size: %d\nBody Size: %d\nTail Size: %d\nOffset = %d\ntotalLen = %d\n", head.length(), fb.size(), tail.length(), offset, totalLen);
    assert(offset == totalLen);
    free(body);
    fb.close();
    
    Serial.println(client.getString());
    client.end();
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}

void sendCamPhoto() {
  //Setup HTTP Connection to Server
  Serial.print("Connecting to server: " + serverName + " ...");
  while (!client.begin(serverName + serverPath)) {
    delay(100);
  }  
  Serial.printf(" Done.\n");

  //Add Header Fields
  client.addHeader("Host", serverName);
  client.addHeader("Content-Type", "multipart/form-data; boundary=ESP32CAMERAFEED");
  String head = "\n--ESP32CAMERAFEED\r\nContent-Disposition: form-data; name=\"file\"; filename=\"/frame.bmp\"\r\nContent-Type: image/bmp\r\n\r\n";
  String tail = "\r\n--ESP32CAMERAFEED--\r\n";
  uint32_t imageLen = camera->frameBytes + 66;
  uint32_t extraLen = head.length() + tail.length();
  uint32_t totalLen = imageLen + extraLen;
  char s_totalLen[256];
  itoa(totalLen, s_totalLen, 10);
  client.addHeader("Content-Length", s_totalLen);

  //Initialize POST request space in memory 
  uint8_t* body = (uint8_t*)malloc(sizeof(uint8_t)*totalLen);
  assert(body != NULL);
  int offset = 0;

  //Write MIME Head String
  int end = offset + head.length();
  for (int i = offset; i < end; i++) {
    body[i] = head[i];
  }
  offset = end;

  //Write MIME Body (BMP Image)
  //Write BMP Header
  end = offset + 66;
  int j = 0;
  for (int i = offset; i < end; i++) {
    body[i] = bmpHeader[j++];
  }
  offset = end;
  //Write BMP Body
  end = offset + camera->frameBytes;
  j = 0;
  for (int i = offset; i < end; i++) {
    body[i] = camera->frame[j++];
  }
  offset = end;

  //Write MIME Tail String
  end = offset + tail.length();
  j = 0;
  for (int i = offset; i < end; i++) {
    body[i] = tail[j++];
  }
  offset = end;

  //Send POST Request & Close Connection
  int code = client.POST(body, offset);
  assert(offset == totalLen);
  free(body);
  Serial.printf("HTTP Code: %d (%s)\nServer Response: %s\n", code, client.errorToString(code).c_str(), client.getString().c_str());
  client.end();
}

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
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
}

void loop() {
  Serial.printf("Gathering Frame Data ...");
  camera->oneFrame();
  Serial.printf(" Done.\n");
  sendCamPhoto();
  delay(500);
}
