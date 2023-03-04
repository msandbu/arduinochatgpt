#include <WiFi.h>
#include <ArduinoJson.h>
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;
float p = 3.1415926;

// Define your WiFi credentials within the "" below
const char* ssid = "";
const char* password = "";
const char* serverHostname = "api.openai.com";
const int serverPort = 443;

void setup() {
  Serial.begin(9600);

  Serial.print(F("Hello! ST77xx TFT Test"));
  carrier.noCase();
  carrier.begin();

  uint16_t time = millis();
  carrier.display.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  WiFiClient client;

  if (client.connectSSL(serverHostname, serverPort)) {
    Serial.println("Connected to server");


    // Send HTTP POST request and change the prompt to your own
    String requestBody = "{\"model\":\"text-davinci-003\",\"prompt\":\"make a quote\",\"max_tokens\":150}";
    String request = "POST /v1/completions HTTP/1.1\r\n";
    request += "Host: api.openai.com\r\n";
    // Replace with your own API Key Here
    request += "Authorization: Bearer APIKEY\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + String(requestBody.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += requestBody;

    client.print(request);

    // Read response
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
 
    String response = client.readString();
    Serial.println("Response:");
    Serial.println(response);

     // Parse JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.println("Failed to parse JSON");
    } else {
      String completionText = doc["choices"][0]["text"];
      Serial.println("ChatGPT response: "+ completionText);
      // Change the size here to whatever you want (1-4) 2 is lower
      carrier.display.setTextSize(3);
      carrier.display.println("ChatGPT Respons" + completionText);
      }
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  // Do nothing
}
