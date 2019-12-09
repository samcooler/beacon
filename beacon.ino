#include <OctoWS2811.h>

const int ledsPerStrip = 180;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

const int numSpots = 10;
const int spotWidth = 4;
const float boundary = 0.01;

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

void setup() {
  //  Serial.begin(57600);
  pinMode(13, OUTPUT);
  randomSeed(analogRead(0));

  // init spots
  int centerColor = random(360);
  for (int i = 0; i < numSpots; i++) {
    spots[i].location = float(random(100)) / 100.0;
    spots[i].velocity = 0.003 * (2 * random(2) - 1.0);
    spots[i].acceleration = 0.0;
    //    spots[i].color = (centerColor + colors[i % 4] + (2*random(2) - 1) * random(20)) % 360;
    //    spots[i].color = (colors[i % 4] + (2*random(2) - 1) * random(20)) % 360;
    spots[i].color = random(360);
  }


  leds.begin();
  for (int i = 0; i < ledsPerStrip * 8; i++) {
    leds.setPixel(i, makeColor(centerColor, 100, 50));
  }
  leds.show();
  delay(1000);
  for (int i = 0; i < ledsPerStrip * 8; i++) {
    leds.setPixel(i, 0, 0, 0);
  }
  leds.show();
}

void loop() {
  //  digitalWrite(13, HIGH);
  //  delay(10);
  //  digitalWrite(13, LOW);
  for (int i = 0; i < ledsPerStrip * 8; i++) {
    leds.setPixel(i, 0, 0, 0);
  }
  for (int i = 0; i < numSpots; i++) {

    spots[i].jerk = 0.0005 * (2 * random(2) - 1.0);

    if (spots[i].location < boundary) {
      if (spots[i].velocity < 0) {
        spots[i].acceleration = 0.007;
      }
    }
    if (spots[i].location > 1.0 - boundary) {
      if (spots[i].velocity > 0) {
        spots[i].acceleration = -0.007;
      }
    }

    spots[i].move();
    spots[i].acceleration = spots[i].acceleration * 0.6;
    spots[i].velocity = spots[i].velocity * 0.97;

    //    if(spots[i].velocity == 0.0) {
    //
    //      digitalWrite(13, HIGH);
    //      delay(10);
    //      digitalWrite(13, LOW);
    //    }
    int ledLocation = spots[i].location * ledsPerStrip;
    //    Serial.println(spots[i].location, 5);
    //    Serial.println(spots[i].velocity, 5);
    //    Serial.println(spots[i].acceleration, 5);
    //    Serial.println(spots[i].jerk, 5);
    //    Serial.println();

    float center = spots[i].location * ledsPerStrip;
    int colorOffset = 0;
    float luminance;
    for (int offset = -1 * spotWidth; offset <= spotWidth; offset++) {
      int led = ledLocation + offset;
      if (led >= 0 && led <= ledsPerStrip) {

        luminance = min(1.3 - abs(center - led) / spotWidth, 1.0);
        colorOffset = 0;//10 * (spotWidth - abs(offset));

        leds.setPixel(led, makeColor((spots[i].color + colorOffset) % 360, 100, 50 * luminance));
      }
    }
  }
  leds.show();
  delay(50);
}
