// the sketch is writtain mainly by ChatGPT for CYD 2 USB. Dont forget to set TFT_ESPI User_Setup for your Display

// i have recently started tinkering with microcontroller have no programming knowledge but enjoy TINKERING

// The clock has been adjusted to BST(UK) and updates every second but the Bit coin prices are updated every 5 minutes


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define TFT_WIDTH 320 // landscape mode USB to right
#define TFT_HEIGHT 240 // " "

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000); // Update every minute correct for bst

// TFT display instance
TFT_eSPI tft = TFT_eSPI();

// Cryptocurrency price API URL (BTC and ETH in USD)
const char* cryptoApiUrl = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum&vs_currencies=usd";

// Variables to store cryptocurrency prices
float btcPrice = 0.0;
float ethPrice = 0.0;
unsigned long lastPriceUpdate = 0;

// Function to fetch cryptocurrency prices
bool fetchCryptoPrices(float& btcPrice, float& ethPrice) {
  HTTPClient http;
  http.begin(cryptoApiUrl);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload);
    btcPrice = doc["bitcoin"]["usd"];
    ethPrice = doc["ethereum"]["usd"];
    http.end();
    return true;
  } else {
    http.end();
    return false;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize TFT display
  tft.init();
  tft.setRotation(1); // landscape mode on CYD usb on right
  tft.fillScreen(TFT_BLACK);

  // Draw red border
  tft.drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_RED);
  tft.drawRect(1, 1, TFT_WIDTH - 2, TFT_HEIGHT - 2, TFT_RED);
  tft.drawRect(2, 2, TFT_WIDTH - 4, TFT_HEIGHT - 4, TFT_RED);
  tft.drawRect(3, 3, TFT_WIDTH - 6, TFT_HEIGHT - 6, TFT_RED);
  tft.drawRect(4, 4, TFT_WIDTH - 8, TFT_HEIGHT - 8, TFT_RED);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP client
  timeClient.begin();

  // Initial price fetch
  fetchCryptoPrices(btcPrice, ethPrice);
}

void loop() {
  // Update the time
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();

  // Fetch cryptocurrency prices every 5 minutes
  if (millis() - lastPriceUpdate > 300000) { // 5 minutes in milliseconds
    fetchCryptoPrices(btcPrice, ethPrice);
    lastPriceUpdate = millis();
  }

  // Clear the screen except for the border
  tft.fillRect(5, 5, TFT_WIDTH - 10, TFT_HEIGHT - 10, TFT_BLACK);

  // Display time and date
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  int timeX = (TFT_WIDTH - tft.textWidth(formattedTime)) / 2;
  tft.setCursor(timeX, 20);
  tft.println(formattedTime);

  // Display cryptocurrency prices
  tft.setTextSize(2);
  String btcPriceStr = String("BTC Price: $") + btcPrice;
  String ethPriceStr = String("ETH Price: $") + ethPrice;

  int btcX = (TFT_WIDTH - tft.textWidth(btcPriceStr)) / 2;
  int ethX = (TFT_WIDTH - tft.textWidth(ethPriceStr)) / 2;

  tft.setCursor(btcX, 90);
  tft.println(btcPriceStr);

  tft.setCursor(ethX, 130);
  tft.println(ethPriceStr);

  delay(1000); // Update the clock every second
}
