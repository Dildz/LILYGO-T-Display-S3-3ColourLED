/*********************************************************************************************************
 * LILYGO T-Display-S3 3-Colour LED Project
 *
 * Description:
 *   This code controls a three-colour (RGB) LED connected to a LilyGO T-Display-S3 microcontroller board.
 *   It displays operating mode, colour changes, and button presses directly on the built-in screen using
 *    the TFT_eSPI library.
 *   The project demonstrates embedded programming concepts, including:
 *     - Digital Outputs: Controlling the RGB LED by setting the state of GPIO pins.
 *     - Variables: Storing and manipulating data, such as the current LED colour.
 *     - Enumerations (Enums): Defining named constants for colors and operational modes,
 *        improving code readability.
 *     - Functions: Place code into reusable blocks for better organization and modularity.
 *     - State Machines: Implementing different operational modes (automatic or manual colour changes)
 *        based on the program's current state.
 *     - Button Handling: Using the OneButton library to manage button short and long presses for
 *        responsive user interaction.
 *     - TFT_eSPI Library: Utilising the TFT_eSPI graphics library for direct display control.
 *
 * How It Works:
 *   1. RGB LED Control: The code manages an RGB LED, capable of producing Red, Green, Blue, Yellow,
 *       Magenta, Cyan, and White by combining RGB colours.
 *   2. Automatic Mode: The LED cycles through colors automatically, changing every second.
 *   3. Manual Mode: The user can manually change the LED colour with each button press.
 *   4. Button Interaction: The onboard "KEY" button (GPIO14) serves two purposes:
 *      - Short Press: In Manual Mode, a short press cycles the LED to the next colour.
 *      - Long Press (1 second): Switches between Automatic and Manual modes.
 *   5. TFT_eSPI Display: The project utilises the TFT_eSPI library for displaying status,
 *       colour information, and button state directly on the screen.
 *
 * Core Concepts and Benefits:
 *   - Helper Functions: Improve code organization, reusability, and maintainability.
 *   - Enums: Enhance code readability by using descriptive names instead of raw numbers.
 *   - State Machines: Simplify the management of different program behaviors and modes.
 *   - OneButton Library: Provides a robust way to handle button presses and long presses without
 *      requiring complex interrupt management.
 *   - TFT_eSPI: Allows for direct control of the display, enabling direct feedback and state information
 *      without the need of the serial monitor.
 *
 * Pin Connections:
 *   - Red LED        -> GPIO1
 *   - Green LED      -> GPIO2
 *   - Blue LED       -> GPIO3
 *   - Button         -> GPIO14 (built-in KEY button)
 *   - LCD Backlight  -> GPIO15
 *   - Ground         -> GND
 *
 * Notes:
 *   - The OneButton library simplifies button handling by debouncing and detecting short and long presses.
 *   - The TFT_eSPI library is configured to work with the LilyGO T-Display-S3, providing an easy way to
 *      display information on the built-in screen.
 *   - The code uses a state machine to manage the automatic and manual colour change modes, ensuring
 *      clean and maintainable logic.
 *   - Helper functions are moved into a .h file which is then included in main.cpp to make the project
 *      even "cleaner" and easier to maintain.
 *********************************************************************************************************/


/*************************************************************
******************* INCLUDES & DEFINITIONS *******************
**************************************************************/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <OneButton.h>

#include "helper_functions.h" // include the helper functions

// OneButton link
OneButton keyButton(BUTTON_PIN, true); // button object

// TFT_eSPI
TFT_eSPI tft = TFT_eSPI();


/*************************************************************
*********************** MAIN FUNCTIONS ***********************
**************************************************************/

// SETUP
void setup() {
  // Initialize the RGB LED pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Initialise the backlight pin
  pinMode(PIN_LCD_BL, OUTPUT);

  // Set the button pin as input with pullup resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP); // active LOW (HIGH when not pressed)

  // Initialize the LED to the starting colour
  setLEDColour(currentColour);

  // TFT_eSPI setup
  tft.init();
  tft.setRotation(0); // adjust rotation (0 & 2 portrait | 1 & 3 landscape)
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2); // set the font size
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text colour (white) and background colour (black)

  // Draw static elements once
  drawStaticElements(tft);

  // Set up button event handlers
  keyButton.attachClick(onShortPress);
  keyButton.attachLongPressStart(onLongPressStart);
  keyButton.setPressMs(1000); // set long press duration in ms

  redrawDisplay = true; // update screen
}

// MAIN LOOP
void loop() {
  // Keep the button state updated
  keyButton.tick();
  bool newButtonState = !digitalRead(BUTTON_PIN);

  if (newButtonState != buttonPressed) {
    buttonPressed = newButtonState;
    redrawDisplay = true; // update screen
  }

  // State machine logic
  switch (currentState) {
    case State::Colour_CHANGE_AUTO: // automatic colour change
      updateAutoColour();
      break;
    case State::Colour_CHANGE_MANUAL: // manual colour change with button
      break;
    default: // should not happen - reset to default state (Auto Mode)
      currentState = State::Colour_CHANGE_AUTO;
      break;
  }

  // Update the display if something has changed
  if (redrawDisplay) {
    updateDynamicElements(tft);
    redrawDisplay = false; // reset the flag
  }
}
