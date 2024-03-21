#include <Keyboard.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>

#define PIXELPIN   13
#define NUMPIXELS  8
#define MUTE_PIN   10

#define PIN_IN1 14
#define PIN_IN2 15

#define ROTARYSTEPS 2
#define ROTARYMIN 0
#define ROTARYMAX 100


RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);
Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

// Last known rotary position.
int lastPos = -1;

void volumeCalibration(int volume) {
  switch (volume) {
    case 100:
      Keyboard.consumerPress(KEY_MUTE);
      break;
    default:
      Keyboard.consumerPress(KEY_VOLUME_DECREMENT);
      break;
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
void updateNeopixels(int pos) {//This gives the best result
  int position = map(pos , ROTARYMIN ,ROTARYMAX , 0 , NUMPIXELS);
  for (int i = 0; i < NUMPIXELS; i++) {
    if (position == 0) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off all pixels
    } else if (i < position) {
      pixels.setPixelColor(i, pixels.Color(10, 255, 12)); // you can adjust RGB values
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Turn off pixel
    }
  }
  pixels.show();
}
void setup()
{
  //Serial.begin(115200);
  pinMode(MUTE_PIN, INPUT_PULLUP);
  pixels.begin();
  pixels.setBrightness(20);
  Keyboard.begin();
  //while (! Serial);
  //Serial.println("LimitedRotator example for the RotaryEncoder library.");
  encoder.setPosition(0); // start with the value of 0.
  for (int volume = 100; volume >= 0; volume--) { //Reduce volume to zero to match the encoder's position
    volumeCalibration(volume);
    delay(10); 
  }
}


// Read the current position of the encoder and print out when changed.
void loop()
{
  encoder.tick();
  muteUnmute();

  // get the current physical position and calc the logical position
  int newPos = encoder.getPosition() * ROTARYSTEPS;

  if (newPos < ROTARYMIN) {
    encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
    newPos = ROTARYMIN;

  } else if (newPos > ROTARYMAX) {
    encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
    newPos = ROTARYMAX;
  } // if

  if (lastPos != newPos) {
    //Serial.print(newPos);
    //Serial.println();
    
  int volumeDelta = newPos - lastPos;
  
  if (volumeDelta != 0) {
    if (volumeDelta > 0) {
      for (int i = 0; i < volumeDelta; i++) {
        Keyboard.consumerPress(KEY_VOLUME_INCREMENT);
        delay(10);
        Keyboard.consumerRelease();
      }
    } else {
      for (int i = 0; i > volumeDelta; i--) {
        Keyboard.consumerPress(KEY_VOLUME_DECREMENT);
        delay(20);
        Keyboard.consumerRelease();
      }
    }
    updateNeopixels(newPos);
    lastPos = newPos;
   }
  }
}
