#include <CommonBusEncoders.h>

CommonBusEncoders encoders(34, 35, 36, 2);

void setup() {
  encoders.setDebounce(16);
  encoders.resetChronoAfter(1000);
  encoders.addEncoder(1, 2, 22, 1, 100,   0);
  encoders.addEncoder(2, 2, 23, 1, 200, 300);
  Serial.begin(9600);
}

void loop() {
  int index = encoders.readAll();
  if (index != 0) Serial.println(index);
}
