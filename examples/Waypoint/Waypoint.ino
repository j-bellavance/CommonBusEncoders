#include <CommonBusEncoders.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

CommonBusEncoders encoders(3, 2, 10, 3);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

char wpt[5];

void prompt() {
  lcd.setCursor(0, 0);
  lcd.print("Waypoint : _____");
}

char changeChar(int index, int direction) {
  if (wpt[index] == '_' && direction == 1) wpt[index] = 'A';
}

void setup() {
  encoders.setDebounce(16);
  encoders.resetChronoAfter(2000);
  encoders.addEncoder(1, 4, 7, 2, 100,   0);
  encoders.addEncoder(2, 4, 8, 4, 200,   0);
  encoders.addEncoder(3, 4, 9, 1, 300, 399);
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.print("Hello");
  Serial.begin(9600);
}

void loop() {
  int index = encoders.readAll();
  if (index != 0) Serial.println(index);
}
