#include <LiquidCrystal.h>
#include "avr/interrupt.h"
#include "pitches.h"
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int pos = 0;
int GAME_SPEED = 250;
int JUMP_LENGHT = 3;
int DUCK_LENGHT = 3;
int jumpFlag = 0;
int duckFlag = 0;
int interface = 0;
int score = 0;
int cactusX = 16;
int crowX = 16;
int dinoY = 1;

int jPin = 20;
int dPin = 21;
int buzzPin = 30;

int cactusFlag = 0;
int crowFlag = 0;

int mute = 0;

int melodyStart[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5,
  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

int durations[] = {
  8, 8, 8, 4, 4, 4,
  4, 5, 8, 8, 8, 8,
  8, 8, 8, 4, 4, 4,
  4, 5, 8, 8, 8, 8
};

int melodyEnd[] = {

  REST, 2, REST, 4, REST, 8, NOTE_DS4, 8,
  NOTE_E4, -4, REST, 8, NOTE_FS4, 8, NOTE_G4, -4, REST, 8, NOTE_DS4, 8,
  NOTE_E4, -8, NOTE_FS4, 8,  NOTE_G4, -8, NOTE_C5, 8, NOTE_B4, -8, NOTE_E4, 8, NOTE_G4, -8, NOTE_B4, 8,
  NOTE_AS4, 2, NOTE_A4, -16, NOTE_G4, -16, NOTE_E4, -16, NOTE_D4, -16,
  NOTE_E4, 2, REST, 4, REST, 8,
};

int songLength = sizeof(melodyStart) / sizeof(melodyStart[0]);
int notes = sizeof(melodyEnd) / sizeof(melodyEnd[0]) / 2;
int tempo = 120;
int wholenote = (60000 * 4) / tempo;
int divider = 0;
int noteDuration = 0;

byte dino0[8] = {
  B00000,
  B00011,
  B10011,
  B11010,
  B01110,
  B01110,
  B01010,
  B00010,
};

byte dino1[8] = {
  B00000,
  B00011,
  B10011,
  B11010,
  B01110,
  B01110,
  B01010,
  B01000,
};

byte dinoDown1[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B10011,
  B11111,
  B01010,
  B01000,
};

byte dinoDown2[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B10011,
  B11111,
  B01010,
  B00010,
};

byte cactus[8] = {
  B00000,
  B00000,
  B00100,
  B00101,
  B10101,
  B11111,
  B00100,
  B00100,
};
byte bird1[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B01000,
  B01100,
  B01110,
  B11111,
};

byte bird2up[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};

byte bird2down[8] = {
  B01110,
  B01100,
  B01000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};

int button;

void setup() {
  pinMode(jPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(jPin), jump, FALLING);
  attachInterrupt(digitalPinToInterrupt(dPin), duck, CHANGE);
  lcd.begin(16, 2);
  lcd.createChar(0, dino0);
  lcd.createChar(1, dino1);
  lcd.createChar(2, dinoDown1);
  lcd.createChar(3, dinoDown2);
  lcd.createChar(4, cactus);
  lcd.createChar(5, bird1);
  lcd.createChar(6, bird2up);
  lcd.createChar(7, bird2down);

}

void loop() {
  if (interface == 0) {
    lcd.setCursor(4, 0);
    lcd.write("press UP");
    lcd.setCursor(4, 1);
    lcd.write("to start");
    while (!(analogRead(0) >= 60 && analogRead(0) < 200) && !mute) {
      for (int thisNote = 0; thisNote < songLength; thisNote++) {
        int duration = 1000 / durations[thisNote];
        tone(30, melodyStart[thisNote], duration);
        int pause = duration * 1.3;
        delay(pause);
        noTone(30);

      }
      mute = 1;
    }
    if (analogRead(0) >= 60 && analogRead(0) < 200) {
      lcd.clear();
      delay(200);
      interface = 1;
      mute = 0;
    }

  }
  else if (interface == 2) {
    lcd.setCursor(0, 0);
    lcd.print("score:");
    lcd.setCursor(7, 0);
    lcd.print(score - 1);
    lcd.setCursor(0, 1);
    lcd.print("UP to try again");
    while (!(analogRead(0) >= 60 && analogRead(0) < 200) && !mute) {
      for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
        divider = melodyEnd[thisNote + 1];
        if (divider > 0) {
          noteDuration = (wholenote) / divider;
        }
        else if (divider < 0) {
          noteDuration = (wholenote) / abs(divider);
          noteDuration *= 1.5;
          tone(buzzPin, melodyEnd[thisNote], noteDuration * 0.9);
          delay(noteDuration);
          noTone(buzzPin);

        }
      }
      mute = 1;
    }
    if (analogRead(0) >= 60 && analogRead(0) < 200) {
      score = 0;
      lcd.clear();
      delay(200);
      interface = 1;
      mute = 0;
    }
  }
  else {
    if (dPin == LOW) duckFlag = 0;

    if (JUMP_LENGHT == 0 ) {
      JUMP_LENGHT = 3;
      jumpFlag = 0;
      dinoY = 1;
    }
    if (jumpFlag == 1) {
      dinoY = 2;
      JUMP_LENGHT--;
    }

    else if (duckFlag == 1) {
      dinoY = 0;
    }

    else dinoY = 1;

    if (pos > 1) pos = 0;
    printDino(pos, dinoY);
    pos++;
    cactusSpawn();
    if (score + 1 > 3) crawSpawn();
    if (score + 1 > 15) GAME_SPEED = 200;
    if (score + 1 > 30) GAME_SPEED = 150;
    if ((dinoY != 2 && cactusX == 0) || (dinoY) != 0 && crowX == 0 ) {
      crowX = 20;
      cactusX = 22;
      duckFlag = 0;
      jumpFlag = 0;
      GAME_SPEED = 250;
      lcd.clear();
      interface = 2;
    }
  }

  delay(GAME_SPEED);
}

void printDino(int i, int dinoY) {
  lcd.clear();
  if (dinoY < 2) {
    lcd.setCursor(0, 1);
    if (dinoY == 0) lcd.write(byte(i + 2));
    else lcd.write(byte(i));
  }
  else {
    lcd.setCursor(0, 0);
    lcd.write(byte(i));

  }
}
void jump() {
  jumpFlag = 1;
  tone(buzzPin, 2700, 50);
}
void duck() {
  duckFlag = !duckFlag;
  tone(buzzPin, 1300, 50);
}

void cactusSpawn() {
  if (cactusFlag == 0) {
    cactusX = random(16, 26);
    cactusFlag = 1;
  }
  if (cactusX < 17) {
    lcd.setCursor(cactusX - 1, 1);
    lcd.write(4);
    cactusX--;
  }
  else cactusX--;
  if (cactusX == 0) {
    cactusFlag = 0;
    score++;
  }
}

void crawSpawn() {
  if (crowFlag == 0) {
    crowX = random(16, 26);
    if (crowX - cactusX <= 2 && crowX - cactusX >= 0 ) crowX += 4;
    else if (crowX - cactusX >= - 2 && crowX - cactusX <= 0) crowX -= 4;
    crowFlag = 1;
  }
  if (crowX < 17) {
    if (crowX % 2 == 0) {
      lcd.setCursor(crowX - 1, 0);
      lcd.write(5);
    }
    else {
      lcd.setCursor(crowX - 1, 0);
      lcd.write(6);
      lcd.setCursor(crowX - 1, 1);
      lcd.write(7);
    }
    crowX--;
  }
  else crowX--;
  if (crowX == 0) {
    crowFlag = 0;
    score++;
  }
}
