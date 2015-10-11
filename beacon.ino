#include <OctoWS2811.h>

const int ledsPerStrip = 60;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

const int numSpots = 6;
const int spotWidth = 2;
const float boundary = 0.01;

const int millisPerMode = 3000;

class Spot
{
  public:
    Spot();
    int color;
    float location;
    float velocity;
    float acceleration;
    float jerk;
    void move();

};

Spot::Spot()
{
  color = 0;
  location = 0.0;
  velocity = 0.0;
  acceleration = 0.0;
  jerk = 0.0;
}

void Spot::move()
{
  acceleration += jerk;
  velocity += acceleration;
  location += velocity;
  //  while(location > 1.0) location -= 1.0;
  //  while(location < 0) location += 1.0;
}


Spot spots[numSpots];
int colors[4] = {0, 240, 300, 340};
int mode = 0;
int lastModeChangeTime = 0;
int spotsOn = 0;
int colorMode = 0;
int randomColorPerMode = 0;
int dir = 1;

void setup() {
  Serial.begin(57600);
  pinMode(13, OUTPUT);
  randomSeed(analogRead(0));

//  int centerColor = random(360);
  for (int i = 0; i < numSpots; i++) {
    spots[i].location = float(random(10)) / 10.0;
    spots[i].velocity = 0.003 * (2 * random(2) - 1.0);
    spots[i].acceleration = 0.0;
//    spots[i].color = (centerColor + colors[i % 4] + (2 * random(2) - 1) * random(20)) % 360;
    spots[i].color = random(360);
    //    spots[i].color = (colors[i % 4] + (2*random(2) - 1) * random(20)) % 360;
    //    spots[i].color = 240;
  }


  leds.begin();
//  for 
  for (int i = 0; i < ledsPerStrip * 8; i++) {
    leds.setPixel(i, makeColor(1, 100, 100));
  }
  leds.show();
  delay(1000);
//  for (int i = 0; i < ledsPerStrip * 8; i++) {
//    leds.setPixel(i, 50, 0, 50);
//  }
//  leds.show();
//  delay(1000);
}


void loop() {


  
  if(millis() - lastModeChangeTime > random(millisPerMode) + 1000) {
    spotsOn = random(3);
    colorMode = random(7);
    randomColorPerMode = random(360);
    dir = pow(-1, randomColorPerMode % 2);
    lastModeChangeTime = millis();
  }


  
  digitalWrite(13, HIGH);
  delay(1);
  digitalWrite(13, LOW);
  
  int randomColorPerFrame = random(360);
  int luminance = 0;
  int lum = 0;
  int saturation = 0;
  if(spotsOn == 0) {
    luminance = 3;
    saturation = 100;
  } else {
    luminance = 20;
    saturation = 100;
  }
//  colorMode = 4;
//  spotsOn = 0;
  
  int col = 0;
  
  for (int i = 0; i < ledsPerStrip * 8; i++) {
    switch (colorMode) {
      case 0:
        col = ((i + millis() / 40 * dir) % 10) * 14 + randomColorPerMode;
        lum = luminance;
        break;
      case 1:
        col = randomColorPerMode;
        lum = random(luminance);
        break;
      case 2:
        col = random(360);
        lum = random(luminance);
        break;
      case 3:
        col = randomColorPerFrame;
        lum = luminance;
        break;
      case 4:
        col = millis() / 15 + randomColorPerMode;
        lum = luminance;
        break;
      case 5:
        col = randomColorPerMode;
        lum = luminance;
      case 6:
        spotsOn = 0;
        break;
      default:
        col = 0;
        lum = 0;
    }

    leds.setPixel(i, makeColor(col % 360, saturation, lum));
    Serial.println(col);
  }

  
  if(spotsOn == 0) {
  
    for (int i = 0; i < numSpots; i++) {
  
      spots[i].jerk = 0.0007 * (2 * random(2) - 1.0);
  
      if (spots[i].location < boundary) {
        if (spots[i].velocity < 0) {
          spots[i].acceleration = 0.010;
        }
      }
      if (spots[i].location > 1.0 - boundary) {
        if (spots[i].velocity > 0) {
          spots[i].acceleration = -0.010;
        }
      }
  
      spots[i].move();
      spots[i].acceleration = spots[i].acceleration * 0.6;
      spots[i].velocity = spots[i].velocity * 0.99;
  
  
      
      int ledLocation = spots[i].location * ledsPerStrip;
//      Serial.println(spots[i].location, 5);
      //    Serial.println(spots[i].velocity, 5);
      //    Serial.println(spots[i].acceleration, 5);
      //    Serial.println(spots[i].jerk, 5);
      //    Serial.println(ledLocation);
//      Serial.println();
  
  
      /*
      ##########
      # Draw Spots
      ##########
      */

      
      //    for(int strip; strip < 8; strip++) {
      int stripIndex = 0;
      float center = spots[i].location * ledsPerStrip;
//      int colorOffset = 0;
      float luminance;
      
      for (int offset = -1 * spotWidth; offset <= spotWidth; offset++) {
        
        int led = ledLocation + offset + stripIndex * ledsPerStrip;
        if (led >= 0 && led <= ledsPerStrip) {
  
          luminance = min(1.3 - abs(center - led) / spotWidth, 1.0);
//          colorOffset = 0;//10 * (spotWidth - abs(offset));
  
          leds.setPixel(led, makeColor(spots[i].color % 360, 100, 50 * luminance));
  //        Serial.println(":");
  //        Serial.println(led);
  //        Serial.println(makeColor((spots[i].color + colorOffset)% 360, 100, 50));
  //        Serial.println(luminance);
        }
      }
      //    }
    } // mode
  }

//  Serial.println("startline");
//  for (int i = 0; i < ledsPerStrip * 8; i++) {
//    Serial.println(leds.getPixel(i));
//  }
//  Serial.println("endline");
  Serial.println();
  leds.show();

  delay(50);
}
