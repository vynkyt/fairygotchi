#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN_LEFT D0   
#define BTN_MIDDLE D1   
#define BTN_RIGHT D2   

#define SDA_PIN D4  
#define SCL_PIN D5

// The fallout tut uses pin mapping out to GPIOs 
// but my schem's pins are like D0, D1, D2 etc. so I'm putting them as that for now. 
// I'll see if it's valid later
// also whoops accidentally put this section into setup instead of above
// ok step 7 says the D thing is ok yay

struct Pet {
  int hunger;
  int happiness;
  int energy;
  unsigned long age;
};

Pet pet;

// lowkey struggling w where to put the code......

enum Screen {
  SCREEN_MAIN,
  SCREEN_FEED,
  SCREEN_PLAY,
  SCREEN_SLEEP
};

Screen currentScreen = SCREEN_MAIN;

void setup() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_MIDDLE, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // might b 0x3D?
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Tamagotchi Init...");
  display.display();
  delay(1000);

  pet.hunger = 80;
  pet.happiness = 80;
  pet.energy = 80;
  pet.age = 0;
}

unsigned long lastUpdate = 0;

void updatePet() {
  if (millis() - lastUpdate > 5000) {  
    pet.hunger--;
    pet.happiness--;
    pet.energy--;

    if (pet.hunger < 0) pet.hunger = 0;
    if (pet.happiness < 0) pet.happiness = 0;
    if (pet.energy < 0) pet.energy = 0;

    pet.age += 5;
    lastUpdate = millis();
  }
}

unsigned long lastButtonPress = 0;

void checkButtons() {
  if (millis() - lastButtonPress < 200) return;  

  if (digitalRead(BTN_LEFT) == LOW) {
    currentScreen = SCREEN_FEED;
    lastButtonPress = millis();
  }
  else if (digitalRead(BTN_MIDDLE) == LOW) {
    currentScreen = SCREEN_PLAY;
    lastButtonPress = millis();
  }
  else if (digitalRead(BTN_RIGHT) == LOW) {
    currentScreen = SCREEN_SLEEP;
    lastButtonPress = millis();
  }
}

void handleScreenLogic() {
  switch(currentScreen) {

    case SCREEN_FEED:
      pet.hunger += 10;
      if (pet.hunger > 100) pet.hunger = 100;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_PLAY:
      pet.happiness += 10;
      pet.energy -= 5;
      if (pet.happiness > 100) pet.happiness = 100;
      if (pet.energy < 0) pet.energy = 0;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_SLEEP:
      pet.energy += 15;
      if (pet.energy > 100) pet.energy = 100;
      currentScreen = SCREEN_MAIN;
      break;

    case SCREEN_MAIN:
      break; 
  }
}

const unsigned char PROGMEM petHappy[] = {};
const unsigned char PROGMEM petNeutral[] = {};
const unsigned char PROGMEM petSad[] = {};
const unsigned char PROGMEM petSleep[] = {};
// ill do this laterrrrrrr 
// i want rlly cute fairy animations so ill make the art for it ltr
// also oops almost forgot semicolons here. hate c++ for all the semicolons gng

void render() {
  display.clearDisplay();

  const unsigned char* sprite;
  if (pet.hunger < 30 || pet.happiness < 30 || pet.energy < 30) {
    sprite = petSad;
  } else if (pet.hunger > 50 && pet.happiness > 50 && pet.energy > 50) {
    sprite = petHappy;
  } else {
    sprite = petNeutral;
  }

  display.drawBitmap(56, 2, sprite, 16, 16, SSD1306_WHITE);
  // gna change this later

  display.setTextSize(1);

  display.setCursor(0, 24);
  display.print("HUN ");
  drawBar(24, 24, pet.hunger);

  display.setCursor(0, 34);
  display.print("HAP ");
  drawBar(24, 34, pet.happiness);

  display.setCursor(0, 44);
  display.print("ENG ");
  drawBar(24, 44, pet.energy);

  display.setCursor(0, 56);
  display.println("[Feed] [Play] [Sleep]");

  display.display();
}

void drawBar(int x, int y, int value) {
  int barWidth = 100;
  int barHeight = 6;
  int fillWidth = map(value, 0, 100, 0, barWidth);

  display.drawRect(x, y, barWidth, barHeight, SSD1306_WHITE);       
  display.fillRect(x, y, fillWidth, barHeight, SSD1306_WHITE);      
}

void loop() {
  checkButtons();       
  updatePet();          
  handleScreenLogic();  
  render();             
  delay(100);           
}
