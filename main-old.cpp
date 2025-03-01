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
 *********************************************************************************************************/


/*************************************************************
******************* INCLUDES & DECLARATIONS ******************
**************************************************************/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <OneButton.h>

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

// Global variables
LEDColour currentColour = LEDColour::RED;
State currentState = State::Colour_CHANGE_AUTO; // default to Auto Mode
bool buttonPressed = false;

// Define the pins for the RGB LED
#define RED_PIN 1
#define GREEN_PIN 2
#define BLUE_PIN 3
#define BUTTON_PIN 14 // built-in 'KEY' button
#define PIN_LCD_BL 15 // backlight pin for T-Display S3

// OneButton link
OneButton keyButton(BUTTON_PIN, true); // button object

// TFT_eSPI related declarations
TFT_eSPI tft = TFT_eSPI();
bool redrawRequired = true; // flag to indicate if the screen needs to be updated


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
  redrawRequired = true; // update screen
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

  if (currentTime - lastColourChangeTime >= 1000) {
    lastColourChangeTime = currentTime;
    changeColour();
    redrawRequired = true; // update screen
  }
}

// Function to display status information
void displayStatus() {
  static bool lastButtonState = !buttonPressed; // Initialize to a different state to force update

  if (redrawRequired || lastButtonState != buttonPressed) {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    tft.println("Current State:");
    tft.println((currentState == State::Colour_CHANGE_AUTO) ? "AUTO MODE" : "MANUAL MODE");

    tft.println("\nCurrent Colour:");
    const char* colourNames[] = {"RED", "GREEN", "BLUE", "YELLOW", "MAGENTA", "CYAN", "WHITE"};
    tft.println(colourNames[static_cast<int>(currentColour)]);

    tft.println("\nButton State:");
    tft.println(buttonPressed ? "PRESSED" : "NOT PRESSED");

    redrawRequired = false;
    lastButtonState = buttonPressed;
  }
}

// Button event handlers
void onShortPress() {
  if (currentState == State::Colour_CHANGE_MANUAL) {
    changeColour();
  }
  redrawRequired = true; // update screen
}

void onLongPressStart() {
  currentState = (currentState == State::Colour_CHANGE_AUTO) ? State::Colour_CHANGE_MANUAL : State::Colour_CHANGE_AUTO;
  redrawRequired = true; // update screen
}


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
  pinMode(BUTTON_PIN, INPUT_PULLUP); // active LOW (HIGH when not pressed

  // Initialize the LED to the starting colour.
  setLEDColour(currentColour);

  // TFT_eSPI setup
  tft.init();
  tft.setRotation(1); // adjust rotation if needed
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2); // set the font (you can experiment with different fonts)
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color (white) and background color (black)

  // Set up button event handlers
  keyButton.attachClick(onShortPress);
  keyButton.attachLongPressStart(onLongPressStart);
  keyButton.setPressMs(1000); // set long press duration in ms

  redrawRequired = true; // update screen
}

// MAIN LOOP
void loop() {
  // Keep the button state updated
  keyButton.tick();
  bool newButtonState = !digitalRead(BUTTON_PIN);

  if (newButtonState != buttonPressed) {
    buttonPressed = newButtonState;
    redrawRequired = true; // update screen
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

  displayStatus(); // only update the display if something has changed.

  if (currentState == State::Colour_CHANGE_AUTO) {
    updateAutoColour();
  }
}
