// main.cpp
#include <TFT_eSPI.h>
#include <Graphics.h>
#include <Spaceship.h>
#include <Asteroid.h>
#include <Shot.h>

#define BUTTON_PIN 23

TFT_eSPI tft = TFT_eSPI();
const int potPin = 36;  // Potentiometer pin (other to ground)

unsigned long currentTime = 0;
unsigned int game_time = 0;
int last_loop_time = 0;
int current_loop_time = 0;

// Button shooting (press when High>>Low)
int lastState = LOW;
int currentState;

int Spaceship_life;

void setup() {
  //Serial.begin(115200); //debugging serial
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    initializeSpaceship(tft);
    initializeAsteroids();
    initializeShots();
    Spaceship_life = 3; // Initialize spaceship life
    drawHeart(tft, Spaceship_life); // Initial draw of hearts
}
