/*
 * ESP32-C6 Super Mini - LED Effects
 * 
 * This sketch:
 * - Blinks built-in LED twice every 5 seconds
 * - RGB LED displays pulsing rainbow effect
 * 
 * Note: Battery LED is hardware-controlled by charging IC, not software controllable
 */

#include <Adafruit_NeoPixel.h>

// ESP32-C6 Super Mini pin definitions
#define LED_BUILTIN 15
#define RGB_LED_PIN 8

// NeoPixel configuration
#define NUM_PIXELS 1
Adafruit_NeoPixel rgb(NUM_PIXELS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

// Timing variables
unsigned long lastBlinkTime = 0;
unsigned long lastRainbowUpdate = 0;

// Rainbow effect variables
uint16_t rainbowHue = 0;
uint8_t rainbowBrightness = 0;
bool brightnessIncreasing = true;

// Blink state
int blinkCount = 0;
bool blinkInProgress = false;
unsigned long blinkPhaseTime = 0;

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
 
  // Initialize built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // OFF (active-low)
  
  // Initialize RGB LED
  rgb.begin();
  rgb.setBrightness(50);  // Cap brightness at ~20%
  rgb.clear();
  rgb.show();
  
  Serial.println("LEDs initialized.");
  Serial.println("Note: Battery LED is hardware-controlled and cannot be turned off via software.");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle built-in LED blinking (twice every 5 seconds)
  handleBlinkLED(currentTime);
  
  // Handle RGB rainbow pulsing effect
  handleRainbowPulse(currentTime);
}

void handleBlinkLED(unsigned long currentTime) {
  // Start blink sequence every 5 seconds
  if (!blinkInProgress && (currentTime - lastBlinkTime >= 5000)) {
    blinkInProgress = true;
    blinkCount = 0;
    blinkPhaseTime = currentTime;
    lastBlinkTime = currentTime;
  }
  
  // Handle blink sequence (2 blinks)
  if (blinkInProgress) {
    unsigned long elapsed = currentTime - blinkPhaseTime;
    
    // Each blink: 100ms ON, 100ms OFF
    if (blinkCount < 2) {
      if (elapsed < 100) {
        digitalWrite(LED_BUILTIN, LOW);   // ON (active-low)
      } else if (elapsed < 200) {
        digitalWrite(LED_BUILTIN, HIGH);  // OFF
      } else {
        blinkCount++;
        blinkPhaseTime = currentTime;
      }
    } else {
      blinkInProgress = false;
      digitalWrite(LED_BUILTIN, HIGH);  // Ensure OFF
    }
  }
}

void handleRainbowPulse(unsigned long currentTime) {
  // Update every 20ms for smooth animation
  if (currentTime - lastRainbowUpdate >= 20) {
    lastRainbowUpdate = currentTime;
    
    // Update hue for rainbow cycling
    rainbowHue += 256;  // Cycle through all colors
    if (rainbowHue >= 65536) {
      rainbowHue = 0;
    }
    
    // Update brightness for pulsing effect
    if (brightnessIncreasing) {
      rainbowBrightness += 5;
      if (rainbowBrightness >= 255) {
        rainbowBrightness = 255;
        brightnessIncreasing = false;
      }
    } else {
      if (rainbowBrightness <= 5) {
        rainbowBrightness = 0;
        brightnessIncreasing = true;
      } else {
        rainbowBrightness -= 5;
      }
    }
    
    // Convert HSV to RGB and apply brightness
    uint32_t color = rgb.ColorHSV(rainbowHue, 255, rainbowBrightness);
    rgb.setPixelColor(0, color);
    rgb.show();
  }
}
