#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PWM_INPUT_PIN 4 //Gpio pin 4 used

//oled screen adjust
#define oled_width 128
#define oled_height 64
#define sda_pin 5
#define scl_pin 6
#define oled_i2c_address 0x3C
Adafruit_SSD1306 display(oled_width, oled_height, &Wire, -1);

volatile unsigned long risingedgetime = 0;
volatile unsigned long fallingedgetime = 0;
volatile unsigned long lastperiod = 0;
volatile unsigned long lasthightime = 0;
volatile bool newvalue = false;

void IRAM_ATTR handleinterrupt() {
    unsigned long now = micros();

    if (digitalRead(PWM_INPUT_PIN) == HIGH) {
        risingedgetime = now;
        if (fallingedgetime > 0 && (risingedgetime > fallingedgetime)) {
            unsigned long period = risingedgetime - fallingedgetime;
            if (period > 500) { 
                lastperiod = period;
            }
        }
    } else {
        fallingedgetime = now;
        if (risingedgetime > 0 && (fallingedgetime > risingedgetime)) {
            unsigned long highperiod = fallingedgetime - risingedgetime;
            if (highperiod < lastperiod) { 
                lasthightime = highperiod;
                newvalue = true;
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(sda_pin, scl_pin);
    if (!display.begin(SSD1306_SWITCHCAPVCC, oled_i2c_address)) {
        Serial.println(F("OLED initialization failed!"));
        while (1);
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Starting");
    display.display();
    delay(2000);
  
    pinMode(PWM_INPUT_PIN, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(PWM_INPUT_PIN), handleinterrupt, CHANGE);
}

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);
    if (newvalue) {
        noInterrupts(); 
        unsigned long period = lastperiod;
        unsigned long highTime = lasthightime;
        newvalue = false;
        interrupts(); // Re-enable interrupts

        if (period > 0) {
            float frequency = 1000000.0 / period; // Frequency in Hz
            float dutyCycle = (highTime * 100.0) / period; // Duty cycle as percentage

            if (dutyCycle >= 0.0 && dutyCycle <= 100.0) { 
                Serial.print("Frequency: ");
                Serial.print(frequency, 2);
                Serial.print(" Hz, Duty Cycle: ");
                Serial.print(dutyCycle, 2);
                Serial.println(" %");

                // Display on OLED
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1); 
                display.printf("PWM Read by Varghese Thomas \n \n"); 
                display.printf("Frequency: %.2f Hz\n", frequency); 
                display.printf("Duty Cycle: %.2f %%\n", dutyCycle); 
                display.display(); 
            } else {
                Serial.println("Invalid signal detected: Duty cycle out of range.");
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1);
                display.println("Invalid signal detected");
                display.display();
            }
        } else {
            Serial.println("No valid signal detected: Period is zero.");
        }
    }

    delay(100); 
}
