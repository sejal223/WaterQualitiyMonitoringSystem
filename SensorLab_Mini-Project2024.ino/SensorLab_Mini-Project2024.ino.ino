#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Change the I2C address (0x27) if necessary

float calibration_value = 23.34;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

float ph_act;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long previousMillis = 0;
const long interval = 500L; // Interval in milliseconds

#define SERIAL Serial
#define sensorPin A1
#define DHTPIN 4     
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);
int sensorValue = 0;
float tdsValue = 0;
float Voltage = 0;

void setup() {
    SERIAL.begin(9600);
    previousMillis = millis();
    dht.begin();
    Wire.begin();
  lcd.init();                      // Initialize the LCD
  lcd.backlight();                 // Turn on the backlight
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Initialize previousMillis to the current time
  previousMillis = millis();
}

void loop() {
    sensorValue = analogRead(sensorPin);
    Voltage = sensorValue * 5.0 / 1024.0;
    tdsValue = (133.42 / Voltage * Voltage * Voltage - 255.86 * Voltage * Voltage + 857.39 * Voltage) * 0.5;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        for (int i = 0; i < 10; i++) {
            buffer_arr[i] = analogRead(A0);
            delay(30);
        }

        for (int i = 0; i < 9; i++) {
            for (int j = i + 1; j < 10; j++) {
                if (buffer_arr[i] > buffer_arr[j]) {
                    temp = buffer_arr[i];
                    buffer_arr[i] = buffer_arr[j];
                    buffer_arr[j] = temp;
                }
            }
        }

        avgval = 0;
        for (int i = 2; i < 8; i++)
            avgval += buffer_arr[i];

        float volt = (float)avgval * 5.0 / 1024 / 6;

        ph_act = -5.70 * volt + calibration_value;

        delay(500);
        Serial.print("Voltage: ");
        Serial.println(volt);
        Serial.print("pH Value: ");
        Serial.println(ph_act);
        Serial.print("TDS Value = "); 
        Serial.print(tdsValue);
        Serial.println(" ppm");
        Serial.print(F("Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.print(F("°C "));
        Serial.print(f);
        Serial.print(F("°F  Heat index: "));
        Serial.print(hic);
        Serial.print(F("°C "));
        Serial.print(hif);
        Serial.println(F("°F"));
        Serial.println();
        
        // Check if pH is suitable for drinking water
        if (ph_act >= 6.5 && ph_act <= 7.5) {
            Serial.println("Water is suitable for drinking water.");
             lcd.setCursor(0, 0);
            lcd.print("Drinkable");
        } else {
            Serial.println("Water is not suitable for drinking.");
             lcd.setCursor(0, 0);
            lcd.print("Not Drinkable");
        }
        //unsigned long currentMillis = millis();
delay(2000);
    }
}
