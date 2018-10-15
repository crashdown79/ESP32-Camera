#include "OV2640.h"
#include "esp_log.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <WiFiClient.h>

OV2640 cam;
WebServer server(80);
const char *ssid = "xinyuan";      // Put your SSID here
const char *password = "12345678"; // Put your PASSWORD here

/*

https://imys.net/20180703/multipart-x-mixed-replace.html

0:
HTTP/1.1 200 OK
Content-type: multipart/x-mixed-replace; boundary=123456789000000000000987654321
1-~~~~~~~..
Content-length: 21600
Content-type: image/jpg


*/
void handle_jpg_stream(void)
{
  String length = "";
  size_t size = 0;
 
  WiFiClient client = server.client();
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (1)
  {
    cam.run();
    if (!client.connected())
      break;
    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);

    client.write((char *)cam.getfb(),cam.getSize());
    if (!client.connected())
      break;
  }
}

void setup()
{
  esp_log_level_set("*", ESP_LOG_DEBUG);
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }
  camera_config_t camera_config;
  camera_config.ledc_channel = LEDC_CHANNEL_0;
  camera_config.ledc_timer = LEDC_TIMER_0;
  camera_config.pin_d0 = 17;
  camera_config.pin_d1 = 35;
  camera_config.pin_d2 = 34;
  camera_config.pin_d3 = 5;
  camera_config.pin_d4 = 39;
  camera_config.pin_d5 = 18;
  camera_config.pin_d6 = 36;
  camera_config.pin_d7 = 19;
  camera_config.pin_xclk = 27;
  camera_config.pin_pclk = 21;
  camera_config.pin_vsync = 22;
  camera_config.pin_href = 26;
  camera_config.pin_sscb_sda = 25;
  camera_config.pin_sscb_scl = 23;
  camera_config.pin_reset = 15;
  camera_config.xclk_freq_hz = 20000000;

  cam.init(camera_config);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handle_jpg_stream);

  server.begin();
}

void loop()
{
  server.handleClient();
  // server.handleClient();
}
