#include <Arduino.h> // Always include Arduino.h

// Standard librarys
#include <EEPROM.h>
#include <Wire.h>

// Installed librarys via platformio.ini
// ...

// Project related librarys
// ...

void (*resetFunc)(void) = 0; // declare reset fuction at address 0

// Eeprom
#ifndef EEPROM_ADDRESS_FIRST_RUN_DONE_KEY
#define EEPROM_ADDRESS_FIRST_RUN_DONE_KEY 32 // Eeprom address
#endif
#ifndef EEPROM_ADDRESS_CONFIG
#define EEPROM_ADDRESS_CONFIG 64 // Eeprom address
#endif

// Button
#ifndef POWER_BUTTON_PIN
#define POWER_BUTTON_PIN PIN_PA2 // Button input pin active LOW
#endif
#ifndef POWER_BUTTON_DEBOUNCE_TIME
#define POWER_BUTTON_DEBOUNCE_TIME 50 // Value in milliseconds(ms)
#endif
#ifndef POWER_BUTTON_HOLD_TIME
#define POWER_BUTTON_HOLD_TIME 2000 // Value in milliseconds(ms)
#endif
volatile uint32_t powerButtonFirstTime = millis();
volatile bool powerButtonLastState = HIGH;

// Pwm
#define LED_PWM_STEPS 101
const uint8_t pwmTable[LED_PWM_STEPS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 10, 10, 11, 11, 12, 13, 14, 15, 15, 16, 17, 18, 20, 21, 22, 23, 25, 26, 28, 29, 31, 33, 35, 37, 39, 42, 44, 47, 49, 52, 55, 59, 62, 66, 70, 74, 78, 83, 87, 92, 98, 103, 109, 116, 123, 130, 137, 145, 154, 163, 172, 182, 192, 204, 215, 228, 241, 255};
// https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms

// Power led
#ifndef POWER_LED_FADE_CYCLE_LENGTH
#define POWER_LED_FADE_CYCLE_LENGTH 1000 // Value in milliseconds(ms), 0-5000 (Default 1000)
#endif
volatile uint8_t powerLedFadeDelay;
volatile int8_t powerLedBrightness = 0;
volatile uint32_t powerLedLastFade = millis();
enum
{
    POWER_LED_OFF,
    POWER_LED_ON,
    POWER_LED_FADE_UP,
    POWER_LED_FADE_DOWN,
};
volatile uint8_t powerLedFadeDirection = POWER_LED_OFF;

// Settings
#ifndef FIRST_RUN_DONE_KEY
#define FIRST_RUN_DONE_KEY 0x2E737605 // Unsigned long(uint32) as HEX number
#endif
#ifndef TEST_MODE_DURATION
#define TEST_MODE_DURATION = 5000 // 5 seconds
#endif
enum
{
    POWER_LED_MIN_BRT,
    POWER_LED_MAX_BRT,
    SATA_LED_BRT
};
const uint8_t defaultBrightness[3] = {
    20,  // Default power led min brightness
    100, // Default power led max brightness
    100  // Default sata led brightness
};
volatile uint8_t storedBrightness[3];
volatile uint8_t testBrightness[3];
volatile uint32_t firstRunDoneKey = 0;
bool testMode = false;
volatile uint32_t lastTestModeActivatedMillis = millis();
void resetTestMode()
{
#ifdef DEBUG
    Serial.println(F("[TEST MODE] Reset"));
#endif
    testMode = true;
    lastTestModeActivatedMillis = millis();
}

// RaspberryPi
#ifndef RPI_BUTTON_SIGNAL_LENGTH
#define RPI_BUTTON_SIGNAL_LENGTH 250 // Value in milliseconds(ms)
#endif
volatile uint32_t buttonWriteFirstTime = millis();
volatile bool lastRpiPowerInput = LOW;
volatile uint8_t internalRunlevel = NULL;

// I2C
#define I2C_ADDRESS 0x08 // 8
uint8_t cmd[5];          // 5 bytes for uint32 data
uint8_t indexByte = 0;   // Reset index byte
uint8_t registerToWrite = NULL;
uint32_t lastRegisterReceive = millis(); // Milliseconds[ms]
uint8_t registerWriteTimeout = 100;      // Milliseconds[ms]
// RPi runlevel register
#define I2C_RUNLEVEL_REG 0xA0   // RPi runlevel register: 160
#define I2C_RUNLEVEL_OFF 0x00   // Off byte:                    0
#define I2C_RUNLEVEL_INTER 0x01 // Intermediate byte:           1
#define I2C_RUNLEVEL_READY 0x02 // Ready byte:                   2
// RPi status register
#define I2C_STATUS_REG 0xA1 // RPi runlevel register: 161
// Default brightness register
#define I2C_DEFAULT_PWR_LED_MIN_BRT_REG 0xB0 // Default power led min brightness register:  176
#define I2C_DEFAULT_PWR_LED_MAX_BRT_REG 0xB1 // Default power led max brightness register:  177
#define I2C_DEFAULT_SATA_LED_BRT_REG 0xB2    // Default sata led brightness register:       178
#define I2C_DEFAULT_BRT_STRUCT_REG 0xBF      // Default brightness structure register:      191
// Stored brightness register
#define I2C_STORED_PWR_LED_MIN_BRT_REG 0xC0 // Stored power led min brightness register:    192
#define I2C_STORED_PWR_LED_MAX_BRT_REG 0xC1 // Stored power led max brightness register:    193
#define I2C_STORED_SATA_LED_BRT_REG 0xC2    // Stored sata led brightness register:         194
#define I2C_STORED_BRT_STRUCT_REG 0xCF      // Stored brightness structure register:        207
// Test brightness register
#define I2C_TEST_PWR_LED_MIN_BRT_REG 0xD0 // Test power led min brightness register:    208
#define I2C_TEST_PWR_LED_MAX_BRT_REG 0xD1 // Test power led max brightness register:    209
#define I2C_TEST_SATA_LED_BRT_REG 0xD2    // Test sata led brightness register:         210
#define I2C_TEST_BRT_STRUCT_REG 0xDF      // Test brightness structure register:        223
void process(uint8_t *cmd)
{
    registerToWrite = NULL; // Reset register to write

    switch (cmd[0])
    {
    case I2C_RUNLEVEL_REG: // RPi runlevel register
#ifdef DEBUG
        Serial.printf("[I2C REGISTER] RPi runlevel (%#04X), %i (%#04X)\n\r", cmd[0], cmd[1], cmd[1]);
#endif
        internalRunlevel = cmd[1];
        if (cmd[1] == I2C_RUNLEVEL_INTER)
            powerLedFadeDirection = POWER_LED_FADE_DOWN;
        break;

    case I2C_STORED_PWR_LED_MIN_BRT_REG: // Stored power led min brightness register
    case I2C_STORED_PWR_LED_MAX_BRT_REG: // Stored power led max brightness register
    case I2C_STORED_SATA_LED_BRT_REG:    // Stored sata led brightness register
        if (cmd[1] == 1)                 // Were 1 bytes of data received?
        {
#ifdef DEBUG
            Serial.print(F("[I2C REGISTER] Stored brightness: "));
            Serial.println(cmd[2]);
#endif
            const uint8_t index = cmd[0] - I2C_STORED_PWR_LED_MIN_BRT_REG;
            storedBrightness[index], testBrightness[index] = cmd[2];
            EEPROM.put(EEPROM_ADDRESS_CONFIG, storedBrightness);
            resetTestMode();
            powerLedFadeDelay = round(float(POWER_LED_FADE_CYCLE_LENGTH) / (storedBrightness[POWER_LED_MAX_BRT] - storedBrightness[POWER_LED_MIN_BRT]) / 2);
            switch (powerLedFadeDirection)
            {
            case POWER_LED_ON:
                powerLedFadeDirection = POWER_LED_FADE_DOWN; // Set power led fade down
                break;
            case POWER_LED_OFF:
                powerLedFadeDirection = POWER_LED_FADE_UP; // Set power led fade up
                break;
            default:
                break;
            }
            EEPROM.put(EEPROM_ADDRESS_CONFIG, storedBrightness);
        }
        break;
    case I2C_STORED_BRT_STRUCT_REG: // Stored brightness structure register
        if (cmd[1] == 3)            // Were 3 bytes of data received?
        {
#ifdef DEBUG
            Serial.print(F("[I2C REGISTER] Stored brightness: "));
            Serial.print(cmd[2]);
            Serial.print(F(", "));
            Serial.print(cmd[3]);
            Serial.print(F(", "));
            Serial.println(cmd[4]);
#endif
            for (uint8_t i = 0; i < 3; i++)
                storedBrightness[i], testBrightness[i] = cmd[2 + i];
            EEPROM.put(EEPROM_ADDRESS_CONFIG, storedBrightness);
            resetTestMode();
            powerLedFadeDelay = round(float(POWER_LED_FADE_CYCLE_LENGTH) / (storedBrightness[POWER_LED_MAX_BRT] - storedBrightness[POWER_LED_MIN_BRT]) / 2);
            switch (powerLedFadeDirection)
            {
            case POWER_LED_ON:
                powerLedFadeDirection = POWER_LED_FADE_DOWN; // Set power led fade down
                break;
            case POWER_LED_OFF:
                powerLedFadeDirection = POWER_LED_FADE_UP; // Set power led fade up
                break;
            default:
                break;
            }
        }
        break;

    case I2C_TEST_PWR_LED_MIN_BRT_REG: // Test power led min brightness register
    case I2C_TEST_PWR_LED_MAX_BRT_REG: // Test power led max brightness register
    case I2C_TEST_SATA_LED_BRT_REG:    // Test sata led brightness register
#ifdef DEBUG
        Serial.print(F("[I2C REGISTER] Test brightness: "));
        Serial.println(cmd[1]);
#endif
        const uint8_t index = cmd[0] - I2C_TEST_PWR_LED_MIN_BRT_REG;
        testBrightness[index] = cmd[1];
        resetTestMode();
        powerLedFadeDelay = round(float(POWER_LED_FADE_CYCLE_LENGTH) / (testBrightness[POWER_LED_MAX_BRT] - testBrightness[POWER_LED_MIN_BRT]) / 2);
        switch (powerLedFadeDirection)
        {
        case POWER_LED_ON:
            powerLedFadeDirection = POWER_LED_FADE_DOWN; // Set power led fade down
            break;
        case POWER_LED_OFF:
            powerLedFadeDirection = POWER_LED_FADE_UP; // Set power led fade up
            break;
        default:
            break;
        }
        break;
    case I2C_TEST_BRT_STRUCT_REG: // Test brightness structure register
        if (cmd[1] == 3)          // Were 3 bytes of data received?
        {
#ifdef DEBUG
            Serial.print(F("[I2C REGISTER] Test brightness: "));
            Serial.println(cmd[2]);
            Serial.print(F(", "));
            Serial.print(cmd[3]);
            Serial.print(F(", "));
            Serial.println(cmd[4]);
#endif
            for (uint8_t i = 0; i < 3; i++)
                testBrightness[i] = cmd[2 + i];
            resetTestMode();
            powerLedFadeDelay = round(float(POWER_LED_FADE_CYCLE_LENGTH) / (testBrightness[POWER_LED_MAX_BRT] - testBrightness[POWER_LED_MIN_BRT]) / 2);
            switch (powerLedFadeDirection)
            {
            case POWER_LED_ON:
                powerLedFadeDirection = POWER_LED_FADE_DOWN; // Set power led fade down
                break;
            case POWER_LED_OFF:
                powerLedFadeDirection = POWER_LED_FADE_UP; // Set power led fade up
                break;
            default:
                break;
            }
        }
        break;

    default:
        break;
    }

    registerToWrite = cmd[0];
    lastRegisterReceive = millis();
}
void readI2cData(int byteCount)
{
#ifdef DEBUG
    Serial.print(F("[I2C READ] Raw data: "));
#endif
    // Read i2c bus
    indexByte = 0;
    while (Wire.available())
    {
        const uint8_t tempChar = (uint8_t)Wire.read();
#ifdef DEBUG
        Serial.print(tempChar);
        Serial.print(F(" "));
#endif
        cmd[indexByte++] = tempChar; // Add temp char into command buffer and increment index
    }
#ifdef DEBUG
    Serial.println();
#endif
    if (cmd[0] != NULL) // Process command buffer
        process(cmd);
    for (uint8_t i = 0; i < sizeof(cmd); i++) // Clear command buffer
        cmd[i] = NULL;
}
void sendI2cData()
{
    if (registerToWrite != NULL)
    {
#ifdef DEBUG
        Serial.print(F("[I2C WRITE] Register: "));
        Serial.println(registerToWrite);
#endif
        switch (registerToWrite)
        {
        case I2C_RUNLEVEL_REG: // RPI runlevel register
            // Write internal runlevel
            Wire.write((const uint8_t *)&internalRunlevel, sizeof(internalRunlevel));
            break;
        case I2C_STATUS_REG: // RPI status register
            // Write status runlevel
            Wire.write((const uint8_t *)&internalRunlevel, sizeof(internalRunlevel));
            break;

        case I2C_DEFAULT_PWR_LED_MIN_BRT_REG: // Default power led min brightness register
        case I2C_DEFAULT_PWR_LED_MAX_BRT_REG: // Default power led max brightness register
        case I2C_DEFAULT_SATA_LED_BRT_REG:    // Default sata led brightness register
            // Write default brightness
            Wire.write((const uint8_t *)&defaultBrightness[registerToWrite - I2C_DEFAULT_PWR_LED_MIN_BRT_REG],
                       sizeof(defaultBrightness[registerToWrite - I2C_DEFAULT_PWR_LED_MIN_BRT_REG]));
            break;
        case I2C_DEFAULT_BRT_STRUCT_REG:
            // Write default brightness array
            Wire.write((const uint8_t *)&defaultBrightness, sizeof(defaultBrightness));
            break;

        case I2C_STORED_PWR_LED_MIN_BRT_REG: // Stored power led min brightness register
        case I2C_STORED_PWR_LED_MAX_BRT_REG: // Stored power led max brightness register
        case I2C_STORED_SATA_LED_BRT_REG:    // Stored sata led brightness register
            // Write stored brightness
            Wire.write((const uint8_t *)&storedBrightness[registerToWrite - I2C_STORED_PWR_LED_MIN_BRT_REG],
                       sizeof(storedBrightness[registerToWrite - I2C_STORED_PWR_LED_MIN_BRT_REG]));
            break;
        case I2C_STORED_BRT_STRUCT_REG: // Stored brightness structure register
            // Write stored brightness array
            Wire.write((const uint8_t *)&storedBrightness, sizeof(storedBrightness));
            break;

        case I2C_TEST_PWR_LED_MIN_BRT_REG: // Test power led min brightness register
        case I2C_TEST_PWR_LED_MAX_BRT_REG: // Test power led max brightness register
        case I2C_TEST_SATA_LED_BRT_REG:    // Test sata led brightness register
            // Write test brightness
            Wire.write((const uint8_t *)&testBrightness[registerToWrite - I2C_TEST_PWR_LED_MIN_BRT_REG],
                       sizeof(testBrightness[registerToWrite - I2C_TEST_PWR_LED_MIN_BRT_REG]));
            break;
        case I2C_TEST_BRT_STRUCT_REG: // Test brightness structure register
            // Write test brightness array
            Wire.write((const uint8_t *)&testBrightness, sizeof(testBrightness));
            break;

        default:
            break;
        }
    }
    registerToWrite = NULL; // Reset register to write
}

void setup()
{
    // Debug
#ifdef DEBUG
#if (F_CPU == 16000000L)
    Serial.begin(92160); // 115200 Bitrate at 16mHz
#else
    Serial.begin(115200); // 115200 Bitrate at 20mHz
#endif
    delay(3000); // Wait for debug
    Serial.println();
    Serial.println(F("[SETUP] Start"));
#endif

    // Get first run done key
    EEPROM.get(EEPROM_ADDRESS_FIRST_RUN_DONE_KEY, firstRunDoneKey);
#ifdef DEBUG
    Serial.print(F("[EEPROM] storedKey: 0x"));
    Serial.print(firstRunDoneKey, HEX);
    Serial.print(F(", definedKey: 0x"));
    Serial.println(FIRST_RUN_DONE_KEY, HEX);
#endif
    if (firstRunDoneKey != FIRST_RUN_DONE_KEY)
    {
#ifdef DEBUG
        Serial.println(F("[EEPROM] First run probably never done"));
        Serial.println(F("[EEPROM] Put default brightness into eeprom"));
#endif
        EEPROM.put(EEPROM_ADDRESS_FIRST_RUN_DONE_KEY, FIRST_RUN_DONE_KEY);
        EEPROM.put(EEPROM_ADDRESS_CONFIG, defaultBrightness);
#ifdef DEBUG
        Serial.println(F("[MCU] Restart"));
        delay(200);
#endif
        resetFunc();
    }

    // Get brightness
    EEPROM.get(EEPROM_ADDRESS_CONFIG, storedBrightness);
    powerLedFadeDelay = round(float(POWER_LED_FADE_CYCLE_LENGTH) / (storedBrightness[POWER_LED_MAX_BRT] - storedBrightness[POWER_LED_MIN_BRT]) / 2);
#ifdef DEBUG
    Serial.print(F("const uint8_t pwmTable["));
    Serial.print(LED_PWM_STEPS);
    Serial.print(F("] = {\n"));
    for (uint8_t i = 0; i < LED_PWM_STEPS; i++)
    {
        Serial.print((i % 10 == 0) ? F("     ") : F(" "));      // Print leading space at start of line
        Serial.print(pwmTable[i] < 10 ? " " : "");              // Add space if needed
        Serial.print(pwmTable[i] < 100 ? " " : "");             // Add space if needed
        Serial.print(pwmTable[i]);                              // Print actual value
        Serial.print((i < LED_PWM_STEPS - 1) ? F(",") : F("")); // Print comma after all but last value
        Serial.print((i % 10 == 9) ? F("\n\r") : F(""));        // Print line break every 10 items
    }
    Serial.print(F("};\n\r"));
    Serial.print(F("storedBrightness[SATA_LED_BRT]:\t\t"));
    Serial.println(storedBrightness[SATA_LED_BRT]);
    Serial.print(F("storedBrightness[POWER_LED_MIN_BRT]:\t"));
    Serial.println(storedBrightness[POWER_LED_MIN_BRT]);
    Serial.print(F("storedBrightness[POWER_LED_MAX_BRT]:\t"));
    Serial.println(storedBrightness[POWER_LED_MAX_BRT]);
    Serial.print(F("powerLedFadeDelay:\t\t\t"));
    Serial.println(powerLedFadeDelay);
#endif
    testBrightness[0] = storedBrightness[0];
    testBrightness[1] = storedBrightness[1];
    testBrightness[2] = storedBrightness[2];

    // Power button input
    pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);

    // Power led pin
    pinMode(POWER_LED_PIN, OUTPUT);

    // RaspberryPi
    pinMode(RPI_POWER_INPUT_PIN, INPUT);
    pinMode(RPI_POWER_BUTTON_OUTPUT_PIN, OUTPUT);
    digitalWrite(RPI_POWER_BUTTON_OUTPUT_PIN, HIGH); // Write pin high
    pinMode(RPI_OTHER_BUTTON_OUTPUT_PIN, OUTPUT);
    digitalWrite(RPI_OTHER_BUTTON_OUTPUT_PIN, HIGH); // Write pin high

    // Sata led
    pinMode(SATA_LED_PIN, OUTPUT);
    analogWrite(SATA_LED_PIN, pwmTable[storedBrightness[SATA_LED_BRT]]);

    // I2C
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(readI2cData);
    Wire.onRequest(sendI2cData);

#ifdef DEBUG
    Serial.println(F("[SETUP] Done"));
#endif
}

void loop()
{
    // I2c write register timeout
    if (millis() - lastRegisterReceive > registerWriteTimeout)
        registerToWrite = NULL;

    if (testMode) // Test mode
    {
        // Power led output sequence
        if (powerLedFadeDirection == POWER_LED_FADE_UP || powerLedFadeDirection == POWER_LED_FADE_DOWN)
        {
            const uint32_t powerLedThisFade = millis();
            // if (powerLedThisFade - powerLedLastFade >= POWER_LED_FADE_DELAY)
            if (powerLedThisFade - powerLedLastFade >= powerLedFadeDelay)
            {
                if (powerLedFadeDirection == POWER_LED_FADE_UP)
                {
                    powerLedBrightness++; // Increase power led brightness
                    if (powerLedBrightness > testBrightness[POWER_LED_MAX_BRT])
                    {
                        powerLedBrightness = testBrightness[POWER_LED_MAX_BRT] - 1;
                        powerLedFadeDirection = POWER_LED_FADE_DOWN; // Set power led fade down
                    }
                }
                else if (powerLedFadeDirection == POWER_LED_FADE_DOWN)
                {
                    powerLedBrightness--; // Decrease power led brightness
                    if (powerLedBrightness < testBrightness[POWER_LED_MIN_BRT])
                    {
                        powerLedBrightness = testBrightness[POWER_LED_MIN_BRT] + 1;
                        powerLedFadeDirection = POWER_LED_FADE_UP; // Set power led fade up
                    }
                }
                analogWrite(POWER_LED_PIN, pwmTable[powerLedBrightness]);
                powerLedLastFade = powerLedThisFade;
            }
        }

        // Sata led
        analogWrite(SATA_LED_PIN, pwmTable[testBrightness[SATA_LED_BRT]]);

        // Stop test mode
        if (millis() - lastTestModeActivatedMillis > TEST_MODE_DURATION)
        {
            analogWrite(SATA_LED_PIN, pwmTable[storedBrightness[SATA_LED_BRT]]);
            powerLedFadeDelay = round(float(POWER_LED_FADE_CYCLE_LENGTH) / (storedBrightness[POWER_LED_MAX_BRT] - storedBrightness[POWER_LED_MIN_BRT]) / 2);
            testMode = false;
        }
    }
    else // Normal mode
    {
        // Power button input sequence
        const bool powerButtonThisState = digitalRead(POWER_BUTTON_PIN);
        // Button first pressed
        if (powerButtonLastState == HIGH && powerButtonThisState == LOW)
        {
            powerButtonFirstTime = millis();
#ifdef DEBUG
            Serial.println(F("[BUTTON] First time"));
#endif
        }
        // Power button released
        if (powerButtonLastState == LOW && powerButtonThisState == HIGH)
        {
            // Power button held
            if (millis() - powerButtonFirstTime > POWER_BUTTON_HOLD_TIME)
            {
#ifdef DEBUG
                Serial.println(F("[BUTTON] Held"));
#endif
                if (digitalRead(RPI_POWER_INPUT_PIN) == HIGH) // RPi is powered on
                {
#ifdef DEBUG
                    Serial.println(F("[BUTTON] Held for power off RPi"));
#endif
                    if (millis() - buttonWriteFirstTime > RPI_BUTTON_SIGNAL_LENGTH * 2)
                    {
                        buttonWriteFirstTime = millis();                // Set first time
                        digitalWrite(RPI_POWER_BUTTON_OUTPUT_PIN, LOW); // Write RPi power button pin high
                    }
                }
            }
            // Button pressed
            else if (millis() - powerButtonFirstTime > POWER_BUTTON_DEBOUNCE_TIME)
            {
#ifdef DEBUG
                Serial.println(F("[BUTTON] Pressed"));
#endif
                if (digitalRead(RPI_POWER_INPUT_PIN) == LOW) // RPi is powered off
                {
#ifdef DEBUG
                    Serial.println(F("[BUTTON] Pressed for power on RPi"));
#endif
                    if (millis() - buttonWriteFirstTime > RPI_BUTTON_SIGNAL_LENGTH * 2)
                    {
                        buttonWriteFirstTime = millis();                // Set first time
                        digitalWrite(RPI_POWER_BUTTON_OUTPUT_PIN, LOW); // Write RPi power button pin high
                    }
                }
            }
        }
        powerButtonLastState = powerButtonThisState;

        // RPi power button output sequence
        if (millis() - buttonWriteFirstTime > RPI_BUTTON_SIGNAL_LENGTH) // Time exeeded
            digitalWrite(RPI_POWER_BUTTON_OUTPUT_PIN, HIGH);            // Write RPi power button pin low

        // RPi power input state sequence
        const bool thisRpiPowerInput = digitalRead(RPI_POWER_INPUT_PIN);
        if (thisRpiPowerInput == HIGH && lastRpiPowerInput == LOW)
        {
#ifdef DEBUG
            Serial.println(F("[RPI5] Power state: ON"));
#endif
            lastRpiPowerInput = thisRpiPowerInput;
            internalRunlevel = I2C_RUNLEVEL_INTER;
            powerLedFadeDirection = POWER_LED_FADE_UP;
        }
        else if (thisRpiPowerInput == LOW && lastRpiPowerInput == HIGH)
        {
#ifdef DEBUG
            Serial.println(F("[RPI5] Power state: OFF"));
#endif
            lastRpiPowerInput = thisRpiPowerInput;
            internalRunlevel = I2C_RUNLEVEL_OFF;
        }

        // Power led output sequence
        if (powerLedFadeDirection == POWER_LED_FADE_UP || powerLedFadeDirection == POWER_LED_FADE_DOWN)
        {
            const uint32_t powerLedThisFade = millis();
            // if (powerLedThisFade - powerLedLastFade >= POWER_LED_FADE_DELAY)
            if (powerLedThisFade - powerLedLastFade >= powerLedFadeDelay)
            {
                if (powerLedFadeDirection == POWER_LED_FADE_UP)
                {
                    powerLedBrightness++; // Increase power led brightness
                    if (powerLedBrightness > storedBrightness[POWER_LED_MAX_BRT])
                    {
                        if (internalRunlevel == I2C_RUNLEVEL_READY)
                        {
                            powerLedBrightness = storedBrightness[POWER_LED_MAX_BRT];
                            powerLedFadeDirection = POWER_LED_ON;
                        }
                        else
                        {
                            powerLedBrightness = storedBrightness[POWER_LED_MAX_BRT] - 1;
                            powerLedFadeDirection = POWER_LED_FADE_DOWN; // Set power led fade down
                        }
                    }
                }
                else if (powerLedFadeDirection == POWER_LED_FADE_DOWN)
                {
                    powerLedBrightness--; // Decrease power led brightness
                    if (powerLedBrightness < storedBrightness[POWER_LED_MIN_BRT])
                    {
                        if (internalRunlevel == I2C_RUNLEVEL_OFF)
                        {
                            if (powerLedBrightness <= 0)
                            {
                                powerLedBrightness = 0;
                                powerLedFadeDirection = POWER_LED_OFF;
                            }
                        }
                        else
                        {
                            powerLedBrightness = storedBrightness[POWER_LED_MIN_BRT] + 1;
                            powerLedFadeDirection = POWER_LED_FADE_UP; // Set power led fade up
                        }
                    }
                }
                analogWrite(POWER_LED_PIN, pwmTable[powerLedBrightness]);
                powerLedLastFade = powerLedThisFade;
            }
        }

        // Sata led
        analogWrite(SATA_LED_PIN, pwmTable[storedBrightness[SATA_LED_BRT]]);
    }
}