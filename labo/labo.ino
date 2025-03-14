#include <LCD_I2C.h>
#define BTN_PIN 2

LCD_I2C lcd(0x27, 16, 2);

int led = 8;
unsigned long currentTime;
unsigned long lastTime = 0;
unsigned long checkpoint = 0;
unsigned long checkpoint2 = 0;
int timeBeforeLight = 5000;
int lightPercentage;
int speed = 0;
int brutSpeed = 0;
int direction = 0;
int brutDirection = 0;
int maxSpeed = 120;
int minSpeed = -25;
int leftAngle = -90;
int rightAngle = 90;
bool printState = 1;
int startingPrintTime = 3000;
byte specialCharacter[8] = { B01000, B11000, B01000, B01111, B11101, B00011, B00001, B00111 };
byte front[8] = { B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00000 };
byte back[8] = {B00000, B00100, B00100, B00100, B00100, B10101, B01110, B00100};
long studentNum = 6307713;

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, specialCharacter);
  lcd.createChar(1, front);
  lcd.createChar(2, back);
  lcd.print("Doyon");
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(14, 1);
  lcd.print("13");
  delay(startingPrintTime);
  lcd.clear();
}

void loop() {
  currentTime = millis();

  carMovement();

  lightController();

  printStateFunction();

  print();
}

void print() {
  static int lastTime = 0;
  if (currentTime - lastTime >= 100) {
    Serial.print("etd:");
    Serial.print(studentNum);
    Serial.print(",x:");
    Serial.print(1023 - brutDirection);
    Serial.print(",y:");
    Serial.print(1023 - brutSpeed);
    Serial.print(",sys");
    Serial.print(digitalRead(led));
    Serial.println('.');
    lastTime = currentTime;
    lcd.clear();
    if (printState) {
      lcd.print("Pct lum : ");
      lcd.print(lightPercentage);
      lcd.print('%');
      lcd.setCursor(0, 1);
      lcd.print("Phare: ");
      if (digitalRead(led)) {
        lcd.print("ON");
      } else {
        lcd.print("OFF");
      }
    } else {
      if (speed > 0) {
        lcd.print("Avance : ");
        lcd.print(speed);
        lcd.print("km/h");
      } else if (speed < 0) {
        lcd.print("Recule : ");
        lcd.print(speed);
        lcd.print("km/h");
      } else {
        lcd.print("Immobile");
      }
      lcd.setCursor(0, 1);
      lcd.print("Direction: ");
      lcd.print(direction);
      lcd.print(' ');
      if (direction < 0) {
        lcd.print('G');
      } else if (direction > 0) {
        lcd.print('D');
      } else if (speed < 0) {
        lcd.write(2);
      } else {
        lcd.write(1);
      }
    }
  }
}

void printStateFunction() {
  static int previousState = HIGH;
  static int state = HIGH;
  const int delay = 50;
  static unsigned long lastChange = 0;

  int currentState = digitalRead(BTN_PIN);

  if (currentState != previousState) {
    lastChange = millis();
  }
  previousState = currentState;
  if ((millis() - lastChange) > delay && currentState != state) {
    state = currentState;
    if (state == LOW) {
      printState = !(printState);
    }
  }
}

void carMovement() {
  brutSpeed = analogRead(A2);
  brutDirection = analogRead(A1);

  if (brutSpeed > 523) {
    speed = map(brutSpeed, 523, 1023, 0, minSpeed);
  } else {
    speed = map(brutSpeed, 0, 522, maxSpeed, 0);
  }

  if (brutDirection < 520) {
    direction = map(brutDirection, 0, 519, rightAngle, 0);
  } else {
    direction = map(brutDirection, 520, 1023, 0, leftAngle);
  }
}

void lightController() {
  lightPercentage = map(analogRead(A0), 150, 1023, 0, 100);
  if (lightPercentage < 50) {
    checkpoint2 = 0;
    if (checkpoint == 0) {
      checkpoint = currentTime;
    }
    if (currentTime - checkpoint >= timeBeforeLight) {
      digitalWrite(led, HIGH);
    }
  } else {
    checkpoint = 0;
    if (checkpoint2 == 0) {
      checkpoint2 = currentTime;
    }
    if (currentTime - checkpoint2 >= timeBeforeLight) {
      digitalWrite(led, LOW);
    }
  }
}