#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// WiFi credentials
const char* ssid = "Meters";
const char* password = "m373ring";

// Server configuration
const char* serverURL = "http://10.255.5.134:3000"; // Replace with your server IP
const char* regularEndpoint = "/api/regular";
const char* seniorEndpoint = "/api/senior";

// Button pins
const int BUTTON_A_PIN = D1;  // Regular Queue button
const int BUTTON_B_PIN = D2;  // Senior/PWD Queue button

// LED pins (optional status indicators)
const int LED_WIFI_PIN = D3;    // WiFi status LED
const int LED_SUCCESS_PIN = D4; // Success indicator LED

// Button debouncing
unsigned long lastButtonAPress = 0;
unsigned long lastButtonBPress = 0;
const unsigned long debounceDelay = 1000; // 1 second debounce

// WiFi status
bool wifiConnected = false;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println();
  Serial.println("ESP8266 Queue System Starting...");
  
  // Initialize pins
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(LED_WIFI_PIN, OUTPUT);
  pinMode(LED_SUCCESS_PIN, OUTPUT);
  
  // Turn off LEDs initially
  digitalWrite(LED_WIFI_PIN, LOW);
  digitalWrite(LED_SUCCESS_PIN, LOW);
  
  // Connect to WiFi
  connectToWiFi();
  
  Serial.println("System ready!");
  Serial.println("Button A (D1) -> Regular Queue");
  Serial.println("Button B (D2) -> Senior/PWD Queue");
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    digitalWrite(LED_WIFI_PIN, LOW);
    Serial.println("WiFi disconnected. Attempting to reconnect...");
    connectToWiFi();
    return;
  }
  
  if (!wifiConnected) {
    wifiConnected = true;
    digitalWrite(LED_WIFI_PIN, HIGH);
    Serial.println("WiFi reconnected!");
  }
  
  // Check Button A (Regular Queue)
  if (digitalRead(BUTTON_A_PIN) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonAPress > debounceDelay) {
      lastButtonAPress = currentTime;
      Serial.println("Button A pressed - Regular Queue");
      sendRequest(regularEndpoint, "Regular");
    }
  }
  
  // Check Button B (Senior/PWD Queue)
  if (digitalRead(BUTTON_B_PIN) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonBPress > debounceDelay) {
      lastButtonBPress = currentTime;
      Serial.println("Button B pressed - Senior/PWD Queue");
      sendRequest(seniorEndpoint, "Senior/PWD");
    }
  }
  
  delay(50); // Small delay to prevent excessive polling
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_WIFI_PIN, HIGH);
    wifiConnected = true;
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Retrying...");
    digitalWrite(LED_WIFI_PIN, LOW);
    wifiConnected = false;
  }
}

void sendRequest(const char* endpoint, const char* queueType) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Cannot send request.");
    return;
  }
  
  WiFiClient client;
  HTTPClient http;
  
  String url = String(serverURL) + String(endpoint);
  
  Serial.print("Sending request to: ");
  Serial.println(url);
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  // Flash success LED to indicate sending
  digitalWrite(LED_SUCCESS_PIN, HIGH);
  
  int httpResponseCode = http.POST("{}"); // Empty JSON body
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Response: ");
    Serial.println(response);
    
    if (httpResponseCode == 200) {
      Serial.println(queueType + String(" queue incremented successfully!"));
      // Blink success LED
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_SUCCESS_PIN, HIGH);
        delay(100);
        digitalWrite(LED_SUCCESS_PIN, LOW);
        delay(100);
      }
    } else {
      Serial.println("Server error");
      digitalWrite(LED_SUCCESS_PIN, LOW);
    }
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    digitalWrite(LED_SUCCESS_PIN, LOW);
  }
  
  http.end();
}

// Function to get current queue status (optional)
void getQueueStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Cannot get queue status.");
    return;
  }
  
  WiFiClient client;
  HTTPClient http;
  
  String url = String(serverURL) + "/api/queue";
  
  http.begin(client, url);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Queue Status: ");
    Serial.println(response);
  } else {
    Serial.print("Error getting queue status: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}
