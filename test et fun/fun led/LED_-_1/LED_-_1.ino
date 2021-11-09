#include "FastLED.h"

#define color (light * 15/ 255, 180 * light / 255 , 155* light /255)
float param = 1.5;
const int division = - 1000;

const int COLONNE = 9;
const int LIGNE = 9;
const byte LED_PIN=7;

CRGB leds[COLONNE*LIGNE];

unsigned long timer = 0;
const byte BRIGHTNESS = 240;

void setup() {
//  Serial.begin(115200);
  int ARG_LED=COLONNE*LIGNE;
  LEDS.addLeds<WS2812,LED_PIN,RGB>(leds,ARG_LED);
  LEDS.setBrightness(BRIGHTNESS);
  leds[10] = CRGB (15, 180, 150);
  FastLED.show();
  // delay(500);
}

void loop() {
  for (byte tense=0; tense < COLONNE*LIGNE; tense++) {
    int light= brightness(trans_numX(tense), trans_numY(tense));
    leds[tense] = CRGB color;
//    Serial.print(tense); Serial.print("  ") ; Serial.println(light);
//    FastLED.show();
    }  
    
  Serial.println("show");
  FastLED.show();
}

int trans_numX(byte a_num) {
  int reste=a_num;
  while (reste>=COLONNE){
    reste=reste-COLONNE;
  }
  return reste;  
}

int trans_numY(byte a_num) {
  byte compteur=0;
  int reste=a_num;
  while (reste >= COLONNE){
    reste=reste-COLONNE;
    compteur++;
  }
  return compteur;  
}


int brightness( int Xi, int Yi) {
  int Z=0;
  float temps= millis();
  float X=Xi - COLONNE/2;
  float Y=Yi - LIGNE/2;
  
//  Z= 250 * sin( 0.4*(X*Y)  - (float) (temps/division));
//Z = 250 * sin (1 * (X+Y) - (float) (temps/division));

  
  Z= 250 * sin( param *(sqrt (X*X+Y*Y))  - (float) (temps/division));
  if (Z<0) Z=0;
  if ((X==0)||(Y==0)) Z=0;
  return Z;
}
