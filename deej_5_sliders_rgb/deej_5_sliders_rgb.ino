#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 5
#define DATA_PIN 12

// Define the array of leds
CRGB leds[NUM_LEDS];

// define slider colors

const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};


int analogSliderValues[NUM_SLIDERS];

void setup() { 
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

  delay(500);

  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }

  Serial.begin(9600);
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug

  

  // Set colors for each slider
  // set non-main sliders to Rainbow colors
  for(int i = 1; i < NUM_SLIDERS; i++)
  {
    leds[i] = CHSV((i-1) * (256/(NUM_SLIDERS -1)), 255, 255);
  }

  // adjust brightness depending on slider value
  for(int i = 1; i < NUM_SLIDERS; i++)
  {
    // convert slider values from 0-1024 to 0-256
    float brightness = analogSliderValues[i] / 1024.0;
    // scale brightness quadratic to make it look better
    //brightness *= brightness;
    // scale color 
    leds[i].r *= brightness;
    leds[i].g *= brightness;
    leds[i].b *= brightness;
  }

  // mix all rgb colors together to form the main slider color
  int r_mix = 0;
  int g_mix = 0; 
  int b_mix = 0;

  for(int i = 1; i < NUM_LEDS; i++)
  {
    r_mix += leds[i].r;
    g_mix += leds[i].g;
    b_mix += leds[i].b;
  }
 /*
  // find scale factor for max brightness
  int brightest_color = max(r_mix,max(g_mix, b_mix));


  // scale the mixed color to max brightness
  r_mix *= 1024.0 / brightest_color;
  g_mix *= 1024.0 / brightest_color;
  b_mix *= 1024.0 / brightest_color;
  
  // normalize values to be within 0-255
  r_mix /= (NUM_LEDS - 1);
  g_mix /= (NUM_LEDS - 1);
  b_mix /= (NUM_LEDS - 1);
  */

  r_mix = min(r_mix, 255);
  g_mix = min(g_mix, 255);
  b_mix = min(b_mix, 255);

  // Set brightness to main slider positon 
  float brightness = analogSliderValues[0] / 1024.0;
  // scale brightness quadratic to make it look better
  //brightness *= brightness;
  r_mix = (float) r_mix * (brightness);
  g_mix = (float) g_mix * (brightness);
  b_mix = (float) b_mix * (brightness);

  
  // todo: this currently does not reach max brightness; do some math to make it do 
  // set main slider color to mix of all other sliders
  leds[0] = CRGB(r_mix, g_mix, b_mix);

  FastLED.show();
  
  delay(10);
}

void updateSliderValues() 
{
  for (int i = 0; i < NUM_SLIDERS; i++) 
  {
    // measure the slider 20 times and take the average to smooth out analog noise
    // this reduces flickering with the LEDs
    long accumulatedSliderValue = 0;
    for(int j = 0; j < 20; j ++)
    {
      accumulatedSliderValue += analogRead(analogInputs[i]);
    }
    analogSliderValues[i] = accumulatedSliderValue / 20;
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}
