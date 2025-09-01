#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

// Function prototypes
void connectToWiFi();
void checkWiFiHealth();
bool sendRequestReliable(const char* endpoint, const char* queueType);
void getQueueStatus();

// WiFi credentials
const char* ssid = "Meters";
const char* password = "m373ring";

// Server configuration
const char* serverURL = "https://queueingsystem-kappa.vercel.app";
const char* regularEndpoint = "/api/regular";
const char* seniorEndpoint = "/api/senior";

// Reliability settings - OPTIMIZED FOR STABILITY
const int MAX_RETRIES = 3;
const int RETRY_DELAY = 1000; // Stable 1 second delay
const int HTTP_TIMEOUT = 10000; // Longer timeout for stability
const int CONNECTION_TIMEOUT = 8000; // Longer connection timeout

// Button pins
const int BUTTON_A_PIN = D1;  // Regular Queue button
const int BUTTON_B_PIN = D2;  // Senior/PWD Queue button

// LED pins (optional status indicators)
const int LED_WIFI_PIN = D3;    // WiFi status LED
const int LED_SUCCESS_PIN = D4; // Success indicator LED

// Button debouncing
unsigned long lastButtonAPress = 0;
unsigned long lastButtonBPress = 0;
const unsigned long debounceDelay = 500; // Stable 500ms debounce

// WiFi status management
bool wifiConnected = false;
unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 10000; // Check every 10s

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println();
  Serial.println("ESP8266 Stable Synchronous Queue System Starting...");
  
  // Configure WiFi for better reliability
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
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
  Serial.println("Synchronous mode: Stable and reliable operation!");
}

void loop() {
  // Periodic WiFi health check
  unsigned long currentTime = millis();
  if (currentTime - lastWiFiCheck > wifiCheckInterval) {
    lastWiFiCheck = currentTime;
    checkWiFiHealth();
  }
  
  // Check Button A (Regular Queue) - SYNCHRONOUS
  if (digitalRead(BUTTON_A_PIN) == LOW) {
    if (currentTime - lastButtonAPress > debounceDelay) {
      lastButtonAPress = currentTime;
      Serial.println("Button A pressed - Regular Queue");
      
      // Send request immediately (synchronous)
      if (sendRequestReliable(regularEndpoint, "Regular")) {
        Serial.println("✓ Regular queue request completed!");
      } else {
        Serial.println("✗ Regular queue request failed!");
      }
    }
  }
  
  // Check Button B (Senior/PWD Queue) - SYNCHRONOUS
  if (digitalRead(BUTTON_B_PIN) == LOW) {
    if (currentTime - lastButtonBPress > debounceDelay) {
      lastButtonBPress = currentTime;
      Serial.println("Button B pressed - Senior/PWD Queue");
      
      // Send request immediately (synchronous)
      if (sendRequestReliable(seniorEndpoint, "Senior/PWD")) {
        Serial.println("✓ Senior/PWD queue request completed!");
      } else {
        Serial.println("✗ Senior/PWD queue request failed!");
      }
    }
  }
  
  delay(50); // Standard loop delay
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) { // More attempts
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    digitalWrite(LED_WIFI_PIN, HIGH);
    wifiConnected = true;
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Will retry later...");
    digitalWrite(LED_WIFI_PIN, LOW);
    wifiConnected = false;
  }
}

void checkWiFiHealth() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    digitalWrite(LED_WIFI_PIN, LOW);
    Serial.println("WiFi health check failed. Reconnecting...");
    connectToWiFi();
  } else if (!wifiConnected) {
    wifiConnected = true;
    digitalWrite(LED_WIFI_PIN, HIGH);
    Serial.println("WiFi health check passed!");
  }
  
  // Check signal strength
  int rssi = WiFi.RSSI();
  if (rssi < -80) {
    Serial.print("Warning: Weak WiFi signal: ");
    Serial.print(rssi);
    Serial.println(" dBm");
  }
}

bool sendRequestReliable(const char* endpoint, const char* queueType) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Cannot send request.");
    return false;
  }
  
  String url = String(serverURL) + String(endpoint);
  Serial.print("Sending stable request to: ");
  Serial.println(url);
  
  // LED indication
  digitalWrite(LED_SUCCESS_PIN, HIGH);
  
  // Try multiple times with stable delays
  for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
    Serial.print("Attempt ");
    Serial.print(attempt);
    Serial.print(" of ");
    Serial.println(MAX_RETRIES);
    
    WiFiClientSecure client;
    client.setInsecure(); // For HTTPS without certificate validation
    client.setTimeout(CONNECTION_TIMEOUT);
    
    HTTPClient http;
    http.begin(client, url);
    http.setTimeout(HTTP_TIMEOUT);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP8266-QueueSystem/1.0");
    http.addHeader("Connection", "close");
    
    int httpResponseCode = http.POST("{}");
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      
      if (httpResponseCode == 200) {
        Serial.print("✓ ");
        Serial.print(queueType);
        Serial.println(" queue incremented successfully!");
        Serial.print("Response: ");
        Serial.println(response);
        
        // Success LED indication - simple and stable
        digitalWrite(LED_SUCCESS_PIN, HIGH);
        delay(300);
        digitalWrite(LED_SUCCESS_PIN, LOW);
        delay(100);
        digitalWrite(LED_SUCCESS_PIN, HIGH);
        delay(300);
        digitalWrite(LED_SUCCESS_PIN, LOW);
        
        http.end();
        client.stop();
        return true; // Success!
      }
    } else {
      Serial.print("✗ HTTP Error: ");
      Serial.println(httpResponseCode);
      
      // Print specific error meanings
      switch(httpResponseCode) {
        case -1: Serial.println("  Connection failed"); break;
        case -2: Serial.println("  Send header failed"); break;
        case -3: Serial.println("  Send payload failed"); break;
        case -4: Serial.println("  Not connected"); break;
        case -5: Serial.println("  Connection lost"); break;
        case -6: Serial.println("  No stream"); break;
        case -7: Serial.println("  No HTTP server"); break;
        case -8: Serial.println("  Too less RAM"); break;
        case -9: Serial.println("  Encoding"); break;
        case -10: Serial.println("  Stream write"); break;
        case -11: Serial.println("  Read timeout"); break;
        default: Serial.println("  Unknown error"); break;
      }
    }
    
    http.end();
    client.stop();
    
    // Stable retry delay (only if not last attempt)
    if (attempt < MAX_RETRIES) {
      Serial.print("Retrying in ");
      Serial.print(RETRY_DELAY);
      Serial.println("ms...");
      delay(RETRY_DELAY);
      
      // Check WiFi before retry
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost during retry. Reconnecting...");
        connectToWiFi();
        if (WiFi.status() != WL_CONNECTED) {
          break; // Exit retry loop if WiFi fails
        }
      }
    }
  }
  
  // All attempts failed
  Serial.print("✗ Failed to send ");
  Serial.print(queueType);
  Serial.println(" request after all retries");
  digitalWrite(LED_SUCCESS_PIN, LOW);
  return false;
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
