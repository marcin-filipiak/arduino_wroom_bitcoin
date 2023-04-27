#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "your_ssid";
const char* password = "password";

// Adres URL API
const char* apiUrl = "https://api.coinpaprika.com/v1/tickers/btc-bitcoin";


static const uint8_t bitcoin_logo[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0xE0, 0x00, 0x00, 0x1C, 0xE0, 0x00, 0x00, 0x1C, 0xE0, 0x00,
0x03, 0xFF, 0xE0, 0x00, 0x07, 0xFF, 0xF0, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x00, 0x70, 0xF8, 0x00,
0x00, 0x70, 0x3C, 0x00, 0x00, 0x70, 0x1C, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x70, 0x3E, 0x00,
0x00, 0x70, 0xFC, 0x00, 0x00, 0x7F, 0xF8, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x7F, 0xFE, 0x00,
0x00, 0x78, 0x1F, 0x00, 0x00, 0x70, 0x07, 0x80, 0x00, 0x70, 0x03, 0x80, 0x00, 0x70, 0x03, 0x80,
0x00, 0x70, 0x03, 0x80, 0x00, 0x70, 0x07, 0x80, 0x00, 0x70, 0x07, 0x00, 0x00, 0x70, 0x7E, 0x00,
0x03, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xF8, 0x00, 0x0F, 0xFF, 0xE0, 0x00, 0x00, 0x1C, 0xE0, 0x00,
0x00, 0x1C, 0xE0, 0x00, 0x00, 0x1C, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 };

float pricenow;
float last12h;
float last6h;

void getprice(){
  
  String payload;
 
  // Pobieranie danych JSON z API
  HTTPClient http;
  http.begin(apiUrl);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();

    // Parsowanie danych JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      
    }

    // Wypisanie wartości pola "pricenow"
    pricenow = doc["quotes"]["USD"]["price"];
    last12h = doc["quotes"]["USD"]["percent_change_24h"];
    last6h = doc["quotes"]["USD"]["percent_change_6h"];
    
    //Serial.println(pricenow);
  } else {
    //Serial.print("Błąd zapytania HTTP: ");
    //Serial.println(httpCode);
  }

  http.end();
}

 
void setup() {
  
  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
 
  // Clear the buffer.
  display.clearDisplay();

  // Ustawienie czcionki
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(1, 1);
  display.print("Lacze z WiFi");
  
  display.display();

  // Połączenie z siecią Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.println("Connecting to WiFi...");
  }
  
 
}
 
void loop() {
  // Sprawdzenie dostępności serwera
  if (WiFi.status() == WL_CONNECTED) {

      display.clearDisplay();
      // Ustawienie czcionki
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);


      //logo bitcoina
      display.drawBitmap(0, 0, bitcoin_logo, 32, 32, SSD1306_WHITE);
          
      

      getprice();
      
      // Wyświetlenie pobranej zawartości strony na ekranie
      display.setTextSize(1);
      display.setCursor(45, 15);
      display.print(""+String(pricenow)+"$");

      display.setTextSize(1);
      display.setCursor(5, 40);
      display.print(" 6h: "+String(last6h)+"%");
      display.setCursor(5, 50);
      display.print("12h: "+String(last12h)+"%");

      // Wyświetlenie tekstu na ekranie
      display.display();
  }

  // Pauza
  delay(50000);
  
}
