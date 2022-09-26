#include <Arduino.h>
#include "seven_seg_driver.h"

#define DIM_PIN A6

uint8_t segment_pins[8] = { 4,5,6,7,8,10,11,12 };
uint8_t digit_pins[8] = { 14,15,16,17,18,19,9,13 };

mrc::seven_seg_driver display(segment_pins, digit_pins, 8, true);

void setup() {
  display.write("7SEG.TEST.");
}

void loop() {
  display.draw();
  display.dim_value = map(analogRead(DIM_PIN), 0, 1023, 0, 10);
}