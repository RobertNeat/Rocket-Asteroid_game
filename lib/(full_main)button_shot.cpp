/*
  This is the version of the main.cpp from before refactoring.
  For this file to run correctly:
    - add Bodmer's library tft_Espi and define user_setup
    - connect components correctly like in schematic
  [this is the version with manual button shoting]
*/

#include <TFT_eSPI.h>
#define BUTTON_PIN 23

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display

const int spaceshipWidth = 19;   // Width of the spaceship
const int spaceshipHeight = 14;  // Height of the spaceship

int spaceshipX;    // X-coordinate of the spaceship

int Speed = 2;    // Vertical speed of the spaceship (pixels per loop iteration)
bool movingDown = true;  // Flag to indicate direction of movement (true = downward, false = upward)

const int potPin = 36;  // Potentiometer pin
int potValue = 0; // Potentiometer value

unsigned long currentTime = 0;

const int maxAsteroids = 10;  // Maximum number of active asteroids
unsigned long lastSpawnTime = 0; // Last time an asteroid was spawned
const unsigned long spawnInterval = 600; // Interval between asteroid spawns (in milliseconds)

struct Asteroid {
  int x;      // X-coordinate of the asteroid
  int y;      // Y-coordinate of the asteroid
  bool active;  // Flag to indicate if the asteroid is active
  int hitpoints = 2;  // Hitpoints required to destroy the asteroid
};
Asteroid asteroids[maxAsteroids];  // Array to store active asteroids

struct Spaceship_shot {
  int x;
  int y;
  bool active = false;
};
const int shot_limit = 20;
Spaceship_shot shots[shot_limit];
const unsigned char shotBitmap[] = {
  0x18,
  0x18,
  0x18
};
unsigned long lastShotTime = 0;
const unsigned long shotInterval = 400;//700


unsigned int game_time=0;
int last_loop_time=0;
int current_loop_time=0;

//button shoting
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


const unsigned char spaceshipBitmap[] = {
  0x00, 0xe0, 0x00,   // 00000000 11100000 000
  0x00, 0xa0, 0x00,   // 00000000 10100000 000
  0x03, 0xb8, 0x00,   // 00000011 10111000 000
  0x02, 0x08, 0x00,   // 00000010 00001000 000
  0x02, 0xe8, 0x00,   // 00000010 11101000 000
  0x02, 0xa8, 0x00,   // 00000010 10101000 000
  0x3a, 0xab, 0x80,   // 00111010 10101011 100
  0x2a, 0xea, 0x80,   // 00101010 11101010 100
  0x2a, 0x0e, 0x80,   // 00101110 00001110 100
  0xe0, 0x00, 0xe0,   // 11100000 00000000 111
  0x80, 0x00, 0x20,   // 10000000 00000000 001
  0xfe, 0x0f, 0xe0,   // 11111110 00001111 111
  0x02, 0x08, 0x00,   // 00000010 00001000 000
  0x03, 0xf8, 0x00    // 00000011 11111000 000
};

const unsigned char asteroid_bodyBitmap[] = {
  0x07, 0xe0, // 00000111 11100000
  0x1c, 0x38, // 00011100 00111000
  0x30, 0x0c, // 00110000 00001100
  0x6c, 0x06, // 01101100 00000110
  0x52, 0x02, // 01010010 00000010
  0xd2, 0x03, // 11010010 00000011
  0x8c, 0x01, // 10001100 00000001
  0x80, 0xf1, // 10000000 11110001
  0x81, 0x99, // 10000001 10011001
  0x99, 0x09, // 10011001 00001001
  0xd9, 0x0b, // 11011001 00001011
  0x41, 0x9a, // 01000001 10011010
  0x60, 0xf6, // 01100000 11110110
  0x30, 0x0c, // 00110000 00001100
  0x1c, 0x38, // 00011100 00111000
  0x07, 0xe0  // 00000111 11100000
};

const unsigned char asteroid_tailBitmap[] = {
  0x00, 0x00, // 00000000 00000000
  0x00, 0x00, // 00000000 00000000
  0x00, 0x00, // 00000000 00000000

  0x01, 0x80, // 00000001 10000000
  0x03, 0x80, // 00000011 10000000
  0x03, 0xc0, // 00000011 11000000
  0x03, 0xc2, // 00000011 11000010
  0x0b, 0xe2, // 00001011 11100010
  0x0f, 0xf6, // 00001111 11110110
  0x9f, 0xf7, // 10011111 11110111
  0xdf, 0xff, // 11011111 11111111
  0xd8, 0x1f, // 11011000 00011111
  0xe0, 0x07, // 11100000 00000111
  0xc0, 0x03, // 11000000 00000011
  0x80, 0x01, // 10000000 00000001
  0x80, 0x01, // 10000000 00000001
};

void drawSpaceship() {
  tft.drawBitmap(spaceshipX, tft.height() - 40, spaceshipBitmap, spaceshipWidth, spaceshipHeight, TFT_WHITE, TFT_BLACK);
}

void drawAsteroid(int x, int y) {
  tft.drawBitmap(x, y - 13, asteroid_tailBitmap, 16, 16, TFT_DARKGREY, TFT_BLACK);
  tft.drawBitmap(x, y, asteroid_bodyBitmap, 16, 16, TFT_WHITE, TFT_BLACK);
}

void drawShot(int x, int y) {
  tft.drawBitmap(x, y, shotBitmap, 8, 3, TFT_WHITE, TFT_BLACK);
}

void spawnAsteroid() {
  for (int i = 0; i < maxAsteroids; i++) {
    if (!asteroids[i].active) {
      int asteroidX = random(0, tft.width() - 16); // Assuming asteroid width is 16
      asteroids[i].x = asteroidX;
      asteroids[i].y = 0;  // Start at the top of the screen
      asteroids[i].active = true; // Activate the asteroid
      asteroids[i].hitpoints = 2; // Reset hitpoints
      break;
    }
  }
}

void spawnShot() {
  for (int i = 0; i < shot_limit; i++) {
    if (!shots[i].active) {
      shots[i].x = spaceshipX + 10;
      shots[i].y = tft.height() - 30;  // Start near the spaceship
      shots[i].active = true; // Activate the shot
      break;
    }
  }
}

void setup() {
  Serial.begin(115200); // For serial debugging
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  spaceshipX = (tft.width() - spaceshipWidth) / 2;

  for (int i = 0; i < maxAsteroids; i++) {
    asteroids[i].active = false;
  }

  for (int i = 0; i < shot_limit; i++) {
    shots[i].active = false;
  }
}

void checkCollision() {
  if (potValue > 10) {
    for (int i = 0; i < maxAsteroids; i++) {
      if (asteroids[i].active) {
        if (spaceshipX < asteroids[i].x + 16 && spaceshipX + spaceshipWidth > asteroids[i].x &&
            tft.height() - 40 < asteroids[i].y + 16 && tft.height() - 40 + spaceshipHeight > asteroids[i].y) {

          tft.setCursor((tft.width() / 2) - 56, (tft.height() / 2) - 8);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setTextSize(2);
          tft.println("Game Over!");

          while (1);  // Halt the program after game over
        }
      }

      for (int j = 0; j < shot_limit; j++) {
        if (shots[j].active && asteroids[i].active) {
          if (shots[j].x < asteroids[i].x + 16 && shots[j].x + 8 > asteroids[i].x &&
              shots[j].y < asteroids[i].y + 16 && shots[j].y + 3 > asteroids[i].y) {
            shots[j].active = false; // Deactivate the shot
            asteroids[i].hitpoints--; // Decrement hitpoints

            if (asteroids[i].hitpoints <= 0) {
              tft.fillRect(asteroids[i].x, asteroids[i].y - 14, 18, 32, TFT_BLACK); // Clear asteroid from screen
              asteroids[i].active = false; // Deactivate the asteroid
            }
          }
        }
      }
    }
  }
}

void loop() {
  currentTime = millis();
  currentState = digitalRead(BUTTON_PIN);


  if (currentTime % 30 == 0) { // Base refresh speed

    last_loop_time=current_loop_time;
    current_loop_time=millis();
    int difference= current_loop_time - last_loop_time;

    tft.setCursor(20, 20); // Set cursor to top-left corner
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.print("Time: ");
    tft.print(game_time / 1000); // Convert milliseconds to seconds

    potValue = analogRead(potPin) - 20;
    tft.fillRect(spaceshipX, tft.height() - 40, spaceshipWidth, spaceshipHeight, TFT_BLACK);//clear spaceship
    

    if (potValue <= 10) {
      if (movingDown) {
        spaceshipX += Speed;
      } else {
        spaceshipX -= Speed;
      }

      if (spaceshipX <= 0) {
        movingDown = true;  // Change direction to downward
      } else if (spaceshipX >= tft.width() - spaceshipWidth) {
        movingDown = false;  // Change direction to upward
      }
    } else {
      game_time += difference;
      spaceshipX = (((potValue * 250) / 4075) + 5);  // Set spaceshipX between 5 and 255
    }


    drawSpaceship();  // Draw spaceship at the new position

    if (currentTime - lastSpawnTime >= spawnInterval) {
      spawnAsteroid();
      lastSpawnTime = currentTime; // Update last spawn time
    }


  if (lastState == HIGH && currentState == LOW){
    if (currentTime - lastShotTime >= shotInterval) {
      spawnShot();
      lastShotTime = currentTime; // Update last shot time
    }
  }
    //Serial.println("The button is pressed");
  lastState = currentState;    

    for (int i = 0; i < maxAsteroids; i++) {
      if (asteroids[i].active) {
        tft.fillRect(asteroids[i].x, asteroids[i].y - 13, 16, 29, TFT_BLACK); // Clear previous asteroid position
        asteroids[i].y += 2;  // Move the asteroid downward by 2 pixels

        if (asteroids[i].y > tft.height() + 20) {
          tft.fillRect(asteroids[i].x, asteroids[i].y - 13, 16, 29, TFT_BLACK); // Clear off-screen asteroid
          asteroids[i].active = false; // Deactivate the asteroid when off screen
        } else {
          drawAsteroid(asteroids[i].x, asteroids[i].y); // Draw the asteroid
        }
      }
    }

    for (int i = 0; i < shot_limit; i++) {
      if (shots[i].active) {
        tft.fillRect(shots[i].x, shots[i].y, 8, 3, TFT_BLACK); // Clear previous shot position
        shots[i].y -= 2;  // Move the shot upward by 2 pixels

        if (shots[i].y < 0) {
          shots[i].active = false; // Deactivate the shot when off screen
        } else {
          drawShot(shots[i].x, shots[i].y); // Draw the shot
        }
      }
    }

    checkCollision(); // Check for collision
    
  }
}
