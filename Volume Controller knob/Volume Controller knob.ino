#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include <Keyboard.h>

#define PIXELPIN   22
#define NUMPIXELS  8
#define MUTE_PIN   18 
#define ENCODERA   16
#define ENCODERB   17

Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);
RotaryEncoder encoder(ENCODERA, ENCODERB);
int lastVolume = 0;
int neopixelPosition = 0;

void setup() {
  pixels.begin();
  pixels.setBrightness(5); // Adjust brightness if needed
  
  pinMode(MUTE_PIN, INPUT_PULLUP);
  Keyboard.begin();
  //Serial.begin(115200);
  for (int volume = 100; volume >= 0; volume--) {
    volumeCalebreation(volume);
    delay(10); // Adjust delay as needed
  }
}

void loop() {
  encoder.tick();

  adjustVolume(encoder.getPosition()/2);
  //Serial.print(encoder.getPosition());
  
  muteUnmute();
  //Serial.print(" || ");
  
  int position = map(encoder.getPosition(), 0, 100, 0, NUMPIXELS);
  //position = constrain(position, 0, 100); // Limit position within range
  //Serial.println(position);

  updateNeopixels(position);
}

void volumeCalebreation(int volume) {
  switch (volume) {
    case 100:
      Keyboard.consumerPress(KEY_MUTE);
      break;
    default:
      Keyboard.consumerPress(KEY_VOLUME_DECREMENT);
      break;
  }
}

void adjustVolume(int position) {
  // If position is negative, reset to zero
  if (position < 0) {
    encoder.setPosition(0);
    position = 0;
  }
  
  // If position exceeds 100, reset to 100
  if (position > 100) {
    encoder.setPosition(100);
    position = 100;
  }

  int volumeDelta = position - lastVolume;

  if (volumeDelta != 0) {
    if (volumeDelta > 0) {
      for (int i = 0; i < volumeDelta; i++) {
        Keyboard.consumerPress(KEY_VOLUME_INCREMENT);
        delay(50);
        Keyboard.consumerRelease();
      }
    } else {
      for (int i = 0; i > volumeDelta; i--) {
        Keyboard.consumerPress(KEY_VOLUME_DECREMENT);
        delay(50);
        Keyboard.consumerRelease();
      }
    }
    lastVolume = position;
  }
}




void muteUnmute() {
  int btn = digitalRead(MUTE_PIN);
  if (btn == LOW) {
    // Flash Neopixels
    flashNeopixels();
    
    Keyboard.consumerPress(KEY_MUTE);
    delay(350);
    Keyboard.consumerRelease();
  }
}

void flashNeopixels() {
  // Turn on all Neopixels
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(random(256), random(256), random(256))); // White color, you can adjust RGB values
  }
  pixels.show();
  
  delay(100); // Flash duration
  
  // Turn off all Neopixels
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off pixel
  }
  pixels.show();
}


void updateNeopixels(int position) {//This gives the best result
  for (int i = 0; i < NUMPIXELS; i++) {
    if (position == 0) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off all pixels
    } else if (i < position) {
      pixels.setPixelColor(i, pixels.Color(10, 255, 12)); // White color, you can adjust RGB values
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off pixel
    }
  }
  pixels.show();
}
