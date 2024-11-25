#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

//oled screen adjust
#define oled_width 128
#define oled_height 64

// I2C pins for Seeed XIAO ESP32-S3
#define sda_pin 5
#define scl_pin 6

#define oled_i2c_address 0x3C

Adafruit_SSD1306 display(oled_width, oled_height, &Wire, -1);

String signalQuality(int rssi) {
      if (rssi > -50) return "*****";  // Excellent
      if (rssi > -60) return "****";   // Good
      if (rssi > -70) return "***";    // Fair
      if (rssi > -80) return "**";     // Weak
      return "*";                      // Very Weak
}

String strength(int rssi) {
      if (rssi > -50) return "Excellent";  // Excellent
      if (rssi > -60) return "Good";       // Good
      if (rssi > -70) return "Fair";       // Fair
      if (rssi > -80) return "Weak";       // Weak
      return "Bad";                        // Very Weak
}

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  // Initialize I2C pins
  Wire.begin(sda_pin, scl_pin);
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, oled_i2c_address)) { 
    Serial.println(F("OLED initialization failed!"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  WiFi.mode(WIFI_STA); // Initialize Wi-Fi
  WiFi.disconnect();
  delay(100);

  display.println("Starting");
  display.display();
  delay(2000);
}

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wi-Fi Scanner Program for oled by Varghese Thomas\n");
  display.println("Scanning...");
  display.display();

  int n = WiFi.scanNetworks(); // Scan for Wi-Fi networks
  display.clearDisplay();
  display.setCursor(0, 0);

  if (n == 0) {
    display.println("No networks found!");
  } else {
    display.printf("%d networks found:\n", n);
    display.println("--------------------");

    // Display only the top 5 networks
    for (int i = 0; i < min(n, 5); ++i) {
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);

      display.printf("%d: %s\n", i + 1, ssid.c_str());
      display.printf("%s: [%s]\n", strength(rssi).c_str(), signalQuality(rssi).c_str());
    }
  }

  display.display();

  // Delay before next scan
  delay(10000);
}
