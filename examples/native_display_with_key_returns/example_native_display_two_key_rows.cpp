#include <Arduino.h>
#include "seven_seg_driver.h"

#define DIM_PIN A6
#define TEST_BTN_PIN -1

uint8_t segment_pins[8] = { 4,5,6,7,8,10,11,12 };
uint8_t digit_pins[8] = { 14,15,16,17,18,19,9,13 };
uint8_t return_pins[2] = { 2, 3 };

char display_buffer[20];

mrc::seven_seg_driver display(segment_pins, digit_pins, 8, true, 100, return_pins, 2);

void setup() {
  display.write("7SEG.TEST.");
}

void loop() {
  uint8_t row = analogRead(DIM_PIN) > 512 ? 0 : 1;
  sprintf(display_buffer, "%1d%1d%1d%1d%1d%1d%1d%1d", display.return_buffer[row][0], display.return_buffer[row][1], 
    display.return_buffer[row][2], display.return_buffer[row][3], display.return_buffer[row][4], display.return_buffer[row][5], 
    display.return_buffer[row][6], display.return_buffer[row][7]);

  display.write(display_buffer);
  display.draw();
  //display.dim_value = map(analogRead(DIM_PIN), 0, 1023, 0, 10);
}