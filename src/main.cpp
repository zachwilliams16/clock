
#include <Arduino.h>
#include <SevSegShift.h>

#define SHIFT_PIN_SHCP 6
#define SHIFT_PIN_STCP 5
#define SHIFT_PIN_DS 4
#define BUTTON 7
#define LED A1

SevSegShift sevseg(SHIFT_PIN_DS, SHIFT_PIN_SHCP, SHIFT_PIN_STCP, 1, true);

// Define global variables

// Button variables
int buttonState;
int buttonUpState;

unsigned long lastTimeIncremented = 0;
unsigned long buttonDownSpeed = 25; // how fast you want the button to go when pressed down every x ms
unsigned long lastButtonAttempt = 0;
unsigned long lastSuccessfulButtonAttempt = 0;
unsigned long debounceTime = 30;

unsigned long timeNeededToTurnOffDebounce = 1000;
bool debounceDisabled = false;

unsigned long LargestHour = 9900;
unsigned long LargestMin = 100;

// hour and minutes are added together to get the time to display
unsigned long hour = 0;
unsigned long minute = 0;
unsigned long time = hour + minute;
bool isAM = false; // if the LED should be on or not indicating if it is AM or PM

unsigned long lastMinuteTick = millis();
/**
 * This function tests to see if the bottom two digits need to be set to 0 and increment the upper two digits
 */
void checkMinute()
{
    if (minute > LargestMin - 1)
    {
        minute = 0;
        hour += 100;
    }
    if (hour > LargestHour)
    {
        hour = 100;
    }
    time = hour + minute;
}

/**
 * This function adds 1 to the number being displayed if time mode is on. (not currently in use)
 */
void timeAddMinute()
{
    minute++;
    lastMinuteTick = millis();
    checkMinute();
}

/**
 * this function adds to the number displayed if the button is pressed
 */
void buttonAddMinute()
{
    lastTimeIncremented = millis();
    minute++;
    checkMinute();
}

/**
 * deboucnes the button. and calls buttonAddMinute
 */
void buttonStuff()
{
    if (millis() - lastButtonAttempt > debounceTime) // if last time was at least x ago
    {
        debounceDisabled = false;
        buttonAddMinute();
        lastSuccessfulButtonAttempt = millis();
    }
    else if (millis() - lastSuccessfulButtonAttempt > timeNeededToTurnOffDebounce) // and if last successful press was more than x ago
    {
        debounceDisabled = true;
    }
    if (debounceDisabled) // if button is being held down
    {
        if (millis() - lastTimeIncremented > buttonDownSpeed)
        {
            buttonAddMinute();
        }
    }
    lastButtonAttempt = millis();
}

/**
 * setup code goes here
 */
void setup()
{
    byte numDigits = 4;
    byte digitPins[] = {12, 11, 10, 9};            // These are the PINS of the ** Arduino **
    byte segmentPins[] = {0, 1, 2, 3, 4, 5, 6, 7}; // these are the PINs of the ** Shift register **
    bool resistorsOnSegments = false;              // 'false' means resistors are on digit pins
    byte hardwareConfig = COMMON_CATHODE;          // See README.md for options
    bool updateWithDelays = false;                 // Default 'false' is Recommended
    bool leadingZeros = false;                     // Use 'true' if you'd like to keep the leading zeros
    bool disableDecPoint = false;                  // Use 'true' if your decimal point doesn't exist or isn't connected
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
    sevseg.setBrightness(50);
    pinMode(BUTTON, INPUT_PULLUP);
    Serial.begin(9600);
    buttonUpState = digitalRead(BUTTON);
}

/**
 * put code to be run here
 */
void loop()
{
    sevseg.setNumber(time);
    sevseg.refreshDisplay(); // Must run repeatedly; don't use blocking code (ex: delay()) in the loop() function or this won't work right

    if (digitalRead(BUTTON) != buttonUpState) // if button is pressed set buttonState to pressed and call buttonstuff
    {
        buttonState = digitalRead(BUTTON);
        buttonStuff();
    }
    else // if button is not pressed sets button to not pressed
    {
        buttonState = buttonUpState;
    }
}