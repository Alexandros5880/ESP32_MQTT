#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define GPIO0 0 // Relay
#define GPIO2 2 // LED

// WiFi credentials
const char *ssid = "<wifi ssid>";
const char *password = "<wifi code>";

// MQTT Broker settings
const char* mqtt_server = "<mqtt broker address>"; // MQTT broker address
const int mqtt_port = 1883; // MQTT port
const char* client_id = "<client id not requared>";
const char* my_topic = "<topic and subscription>"; // Topic to subscribe to

// Create WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO0, OUTPUT);
  Serial.begin(115200);
  
  // Connect to WiFi
  setup_wifi();

  // Set up MQTT server and callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(client_id)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(my_topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(my_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void change_relay(bool val)
{
  if (val == 1) {
      digitalWrite(GPIO2, HIGH);
      digitalWrite(GPIO0, HIGH);
      Serial.println("Relay ON");
    } else if (val == 0) {
      digitalWrite(GPIO2, LOW);
      digitalWrite(GPIO0, LOW);
      Serial.println("Relay OFF");
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == String(my_topic)) {
    if (message == "1") {
      change_relay(1);
    } else if (message == "0") {
      change_relay(0);
    }
  }
}
