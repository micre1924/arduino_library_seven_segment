/*
Test-Program made for ESP8266
  # Requirements:
    * 8-Digits 7-SEGMENTS Display
    * 4x3 Key-Matrix
    * Potentiometer
    * LED
*/

#include <Arduino.h>
#include <seven_segment.h>
#include <clk_ticker.h>

#define DISPLAY_SERIAL_PIN D7
#define DISPLAY_SHIFT_PIN D5
#define DISPLAY_LATCH_PIN D6

#define TEST_PWM_OUTPUT D3
#define TEST_ANALOG_INPUT A0

// Return Pins for Key-Matrix
const int8_t returnPins[] = { D0, D1, D2 };

// Key-Matrix Map
const char keyMatrix[4][3] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' },
};

// DIsplay instance
mrc::seven_segment display(
  DISPLAY_SHIFT_PIN, DISPLAY_LATCH_PIN, DISPLAY_SERIAL_PIN, 
  8, false, true, returnPins, 3U
  );
// Buffer for sprintf to format resulting output to display
char testValueBuf[10];

// Variables required for Examples
String testCMD = "";
uint64_t testValue;
String testString;
char returningMatrixChar = 'P';
bool matrixButtonPressedPulse;
bool PTRIG_Pressed;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  // Draw Display
  display.draw();

  // Example for processing Key-Matrix Output
  matrixButtonPressedPulse = false;
  for (size_t x = 0; x < 3 ; x++)
  {
    for (size_t y = 0; y < 4; y++)
    {
      if(display.return_buffer[x][y]){
          returningMatrixChar = keyMatrix[y][x];
          matrixButtonPressedPulse = true;
      }
    }
  }

  if (matrixButtonPressedPulse)
  {
    if (!PTRIG_Pressed)
    {
      PTRIG_Pressed = true;

      // overflow
      if (returningMatrixChar != '*' && returningMatrixChar != '#')
      {
        if (testString.length() < 3)
        {
          testString += returningMatrixChar;
        } else {
          testString = "";
        } 
      }

      // set cmd
      if (returningMatrixChar == '#')
      {
        testCMD = testString;
        testString = "";
      }

      // Delete entry
      if (returningMatrixChar == '*')
      {
        testString = "";
      }
    }
  } else {
    PTRIG_Pressed = false;
  }
  // Example for processing Key-Matrix Output END

  // Example for processing values
  if (testCMD.length() > 0)
  {
    uint32_t cmd = testCMD.toInt();
    
    analogWrite(TEST_PWM_OUTPUT, cmd);
    
    testCMD = "";
  }
  // Example for processing values END

  // Write Data to Display
  sprintf(testValueBuf, "%04d-%3s", analogRead(TEST_ANALOG_INPUT), testString);
  display.write(testValueBuf);

}