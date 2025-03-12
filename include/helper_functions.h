/*************************************************************
************************* DEFINITIONS ************************
**************************************************************/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <TFT_eSPI.h>

// Define the state machine colour states
enum class LEDColour {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  CYAN,
  WHITE
};

// Define the state machine mode states
enum class State {
  Colour_CHANGE_AUTO,   // automatic colour change every second
  Colour_CHANGE_MANUAL  // change colour on button press
};

// Define global variables
LEDColour currentColour = LEDColour::RED;
State currentState = State::Colour_CHANGE_AUTO; // default to Auto Mode
bool buttonPressed = false;
bool redrawDisplay = true;

// Define the pins for the RGB LED
#define RED_PIN 1
#define GREEN_PIN 2
#define BLUE_PIN 3
#define BUTTON_PIN 14 // built-in 'KEY' button
#define PIN_LCD_BL 15 // backlight pin for T-Display S3


/*************************************************************
********************** HELPER FUNCTIONS **********************
**************************************************************/

// Function to set the LED colour
void setLEDColour(LEDColour Colour) {
  switch (Colour) {
    case LEDColour::RED:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case LEDColour::GREEN:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case LEDColour::BLUE:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    case LEDColour::YELLOW:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case LEDColour::MAGENTA:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    case LEDColour::CYAN:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    case LEDColour::WHITE:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    default: // default to red if an unknown colour is specified
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      break;
  }
  redrawDisplay = true; // update screen
}

// Function to change the current colour based on the next colour in the sequence.
void changeColour() {
  currentColour = static_cast<LEDColour>((static_cast<int>(currentColour) + 1) % 7);
  setLEDColour(currentColour);
}

// Function to update LED colours automatically
void updateAutoColour() {
  static unsigned long lastColourChangeTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastColourChangeTime >= 1000) { // 1000ms
    lastColourChangeTime = currentTime;
    changeColour();
    redrawDisplay = true; // update screen
  }
}

// Function to draw static elements on the TFT screen
void drawStaticElements(TFT_eSPI &tft) {
  tft.fillScreen(TFT_BLACK);              // clear the screen
  tft.setTextFont(2);                     // set the font size
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color and background

  // Draw static text or elements
  tft.setCursor(0, 0);
  tft.println("---------------------------");
  tft.println("  3-Colour LED Control");
  tft.println("---------------------------");
  tft.setCursor(0, 70);
  tft.print("Mode: ");
  tft.setCursor(0, 100);
  tft.print("Colour: ");
  tft.setCursor(0, 130);
  tft.print("Button: ");
}

// Function to update dynamic elements on the TFT screen
void updateDynamicElements(TFT_eSPI &tft) {
  // Update the mode (Auto/Manual)
  tft.setCursor(50, 70);
  tft.print("                "); // clear previous text
  tft.setCursor(50, 70);
  tft.print(currentState == State::Colour_CHANGE_AUTO ? "AUTO MODE" : "MANUAL MODE");

  // Update the current LED colour
  tft.setCursor(50, 100);
  tft.print("                "); // clear previous text
  tft.setCursor(50, 100);
  const char* colourNames[] = {"RED", "GREEN", "BLUE", "YELLOW", "MAGENTA", "CYAN", "WHITE"};
  tft.print(colourNames[static_cast<int>(currentColour)]);

  // Update the button state
  tft.setCursor(50, 130);
  tft.print("                "); // clear previous text
  tft.setCursor(50, 130);
  tft.print(buttonPressed ? "PRESSED" : "NOT PRESSED");
}

// Button event handlers
void onShortPress() { // change colours in manual mode with short presses
  if (currentState == State::Colour_CHANGE_MANUAL) {
    changeColour();
  }
  redrawDisplay = true; // update screen
}

void onLongPressStart() { // change modes with a 1sec long press
  currentState = (currentState == State::Colour_CHANGE_AUTO) ? State::Colour_CHANGE_MANUAL : State::Colour_CHANGE_AUTO;
  redrawDisplay = true; // update screen
}

#endif // HELPER_FUNCTIONS_H
