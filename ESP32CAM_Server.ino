//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#define ENABLE_DHT_MQTT

#include "esp_camera.h"
#include "camera_pins.h"
#include "WifiCredentials.h"

#if defined(ENABLE_DHT_MQTT)
#include "DHT22Sensor.h"
#include "MQTT.h"
#endif

#include <cstring>
#include <WiFi.h>

static const unsigned long REBOOT_INTERVAL_MS = 30 * 60 * 1000;

#if defined(ENABLE_DHT_MQTT)
static const unsigned long DHT_READ_INTERVAL_MS = 5 * 60 * 1000;
static unsigned long lastDHTRead = 0;

MyMQTTClient *mqttClient = nullptr;
SimpleDHT22 *dht22 = nullptr;
#endif

void startCameraServer();

void initCamera()
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID)
  {
    s->set_vflip(s, 1);       //flip it back
    s->set_brightness(s, 1);  //up the blightness just a bit
    s->set_saturation(s, -2); //lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_UXGA);
  s->set_aec2(s, 1); //aec dsp
  s->set_gainceiling(s, gainceiling_t::GAINCEILING_4X);
  s->set_lenc(s, 1); //lens correction
  s->set_bpc(s, 1);  //black pixel check
  s->set_wpc(s, 1);  //white pixel check
  s->set_raw_gma(s, 1);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif
}

void connectToWiFi()
{
  WifiCredentials const *const creds = getClosestWifi(WiFi);

  WiFi.begin(creds->SSID, creds->password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

#if defined(ENABLE_DHT_MQTT)
MyMQTTClient *createAndConnectMQTT()
{
  MyMQTTClient *client = new MyMQTTClient();

  if (client->connectToBroker() != MQTT_SUCCESS)
  {
    Serial.printf("failed to connect to MQTT broker! error: %d\n", client->connectError());
  }
  else
  {
    Serial.println("connected to MQTT broker");
  }

  return client;
}
#endif

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println();

  Serial.println("initializing camera");
  initCamera();

  Serial.println("connecting wifi");
  connectToWiFi();

  Serial.println("starting camera server");
  //make sure to establish wifi connection before calling this
  startCameraServer();

  Serial.println("Camera Ready! Use 'http://" + WiFi.localIP().toString() + "' to connect");

#if defined(ENABLE_DHT_MQTT)
  dht22 = new SimpleDHT22(DHT_PIN);
  mqttClient = createAndConnectMQTT();
#endif
}

void loop()
{
  const unsigned long currentTime = millis();

#if defined(ENABLE_DHT_MQTT)
  mqttClient->poll();
#endif

  if (currentTime > REBOOT_INTERVAL_MS)
  {
    Serial.println("restarting");
    ESP.restart();
  }
#if defined(ENABLE_DHT_MQTT)
  else if ((lastDHTRead == 0) || ((currentTime - lastDHTRead) > DHT_READ_INTERVAL_MS))
  {
    float temperature = -1.f;
    float humidity = -1.f;

    getDHTReadings(dht22, temperature, humidity);

    Serial.printf("temperature: %.2f\n", temperature);
    Serial.printf("humidity: %.2f\n", humidity);

    lastDHTRead = currentTime;
    {
      const char MESSAGE_TEMPLATE[] = "{\"t\":%.1f,\"h\":%.1f}";

      String topic = WiFi.macAddress();
      topic.replace(":", "");
      topic.toLowerCase();

      const int stringSize = snprintf(nullptr, 0, MESSAGE_TEMPLATE, temperature, humidity);
      char *const message = new char[stringSize + 1];

      sprintf(message, MESSAGE_TEMPLATE, temperature, humidity);
      mqttClient->sendMessage(topic.c_str(), message);

      delete[] message;
    }
  }
#endif
  else
  {
    delay(2 * 1000);
  }
}
