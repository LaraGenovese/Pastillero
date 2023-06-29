#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define LED1 14 //D5

#define WLAN_SSID       "IoT"
#define WLAN_PASS       "elultimo10"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "LaraGenovese"
#define AIO_KEY         "aio_IDIi07yUHWpvmNrdWfRGGlZUSrYo"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe led = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led-de-prueba");

void MQTT_connect();

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1,OUTPUT);

  Serial.begin(115200);
  //delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&led);
}

uint32_t x=0;

void loop() {
  // put your main code here, to run repeatedly:
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) {
    
    if(subscription == &led){
      Serial.print(F("Estado del boton: "));
      Serial.println((char *)led.lastread);

      if (strcmp((char *)led.lastread, "ON") == 0) {
        digitalWrite(LED1, HIGH);
      }
      if (strcmp((char *)led.lastread, "OFF") == 0) {
        digitalWrite(LED1, LOW);
      }
    }
  }

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
