#include <Arduino.h>
#include <seven_segment.h>

#define DIM_PIN A6
#define TEST_BTN_PIN -1

const int8_t segment_pins[8] = { 4,5,6,7,8,10,11,12 };
const int8_t digit_pins[8] = { 14,15,16,17,18,19,9,13 };

mrc::seven_segment display(segment_pins, digit_pins, 8, true, 100);

void setup() {
  display.write("7SEG.TEST.");
}

void loop() {
  display.draw();
}