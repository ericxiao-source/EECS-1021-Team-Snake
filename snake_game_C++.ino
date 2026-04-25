#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>




// Where the pins are assigned to (on the Arduino2W)
const int OLED_SDA_PIN = 4;    // GP4  physical pin 6
const int OLED_SCL_PIN = 5;    // GP5  physical pin 7
const int LEFT_BUTTON_PIN  = 0;  // GP00 physical pin 0
const int RIGHT_BUTTON_PIN = 1;  // GP01 physical pin 1


// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR    0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Snake board geometry
// 128x64 display -> use 8x8 pixel cells = 16x8 grid
const int CELL_SIZE = 8;
const int BOARD_W = SCREEN_WIDTH / CELL_SIZE;   // 16
const int BOARD_H = SCREEN_HEIGHT / CELL_SIZE;  // 8


// Max snake length = full board
const int MAX_SNAKE_LEN = BOARD_W * BOARD_H;


// Timing
unsigned long lastMoveTime = 0;
const unsigned long MOVE_INTERVAL_MS = 200;


// Game state
struct Point {
  int x;
  int y;
};


Point snake[MAX_SNAKE_LEN];
int snakeLength = 3;
Point food;


// Direction vector
// On the display:
// +x = right
// +y = down
Point dir = {1, 0};   // start moving right


bool gameOver = false;
bool gameStarted = false;


// Button edge handling
bool lastLeftState = HIGH;
bool lastRightState = HIGH;


// Rotation matrices (LINEAR ALGEBRA REFERENCE!!?!?)🤯🤯
// For screen coordinates where +y is downward:
// Left turn  = [ 0  1 ] [x]
//              [-1  0 ] [y]
// Right turn = [ 0 -1 ] [x]
//              [ 1  0 ] [y]


Point rotateLeft(const Point &v) {
  Point out;
  out.x =  0 * v.x + 1 * v.y;
  out.y = -1 * v.x + 0 * v.y;
  return out;
}


Point rotateRight(const Point &v) {
  Point out;
  out.x =  0 * v.x + (-1) * v.y;
  out.y =  1 * v.x + 0 * v.y;
  return out;
}


bool pointEquals(const Point &a, const Point &b) {
  return (a.x == b.x && a.y == b.y);
}


bool snakeContains(const Point &p, int startIndex = 0) {
  for (int i = startIndex; i < snakeLength; i++) {
    if (pointEquals(snake[i], p)) return true;
  }
  return false;
}


Point generateFood() {
  Point p;
  while (true) {
    p.x = random(0, BOARD_W);
    p.y = random(0, BOARD_H);
    if (!snakeContains(p)) {
      return p;
    }
  }
}


void drawCenteredText(const String &line1, const String &line2 = "", const String &line3 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);


  int y = 12;


  if (line1.length()) { 
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, y);
    display.print(line1);
    y += 16;
  }


  if (line2.length()) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, y);
    display.print(line2);
    y += 16;
  }


  if (line3.length()) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(line3, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, y);
    display.print(line3);
  }
  display.display();
}


void drawGame() {
  display.clearDisplay();
  // Draw snake
  for (int i = 0; i < snakeLength; i++) {
    int px = snake[i].x * CELL_SIZE;
    int py = snake[i].y * CELL_SIZE;
    if (i == 0) {
      // head
      display.fillRect(px, py, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);
      display.drawRect(px + 1, py + 1, CELL_SIZE - 2, CELL_SIZE - 2, SSD1306_BLACK);
    } else {
      // body
      display.fillRect(px + 1, py + 1, CELL_SIZE - 2, CELL_SIZE - 2, SSD1306_WHITE);
    }
  }
  // Draw food
  int fx = food.x * CELL_SIZE;
  int fy = food.y * CELL_SIZE;
  display.fillCircle(fx + CELL_SIZE / 2, fy + CELL_SIZE / 2, 2, SSD1306_WHITE);
  display.display();
}


void startNewGame() {
  snakeLength = 3;
  // Start roughly in the middle
  snake[0] = {BOARD_W / 2, BOARD_H / 2};
  snake[1] = {snake[0].x - 1, snake[0].y};
  snake[2] = {snake[0].x - 2, snake[0].y};
  dir = {1, 0};
  food = generateFood();
  gameStarted = true;
  gameOver = false;
  lastMoveTime = millis();
  drawCenteredText("Game started");
  delay(670); // 67 HAHAHHAHAHAHHA
  drawGame();
}


bool anyButtonPressed() {
  return digitalRead(LEFT_BUTTON_PIN) == LOW || digitalRead(RIGHT_BUTTON_PIN) == LOW;
}


void waitForButtonRelease() {
  while (anyButtonPressed()) {
    delay(10);
  }
}


void handleTurnInput() {
  bool leftState = digitalRead(LEFT_BUTTON_PIN);
  bool rightState = digitalRead(RIGHT_BUTTON_PIN);


  // edge detect so one press = one turn
  if (lastLeftState == HIGH && leftState == LOW) {
    dir = rotateLeft(dir);
  } else if (lastRightState == HIGH && rightState == LOW) {
    dir = rotateRight(dir);
  }
  lastLeftState = leftState;
  lastRightState = rightState;
}


void moveSnake() {
  Point newHead = {snake[0].x + dir.x, snake[0].y + dir.y};
  // Collision with wall
  bool hitWall = (newHead.x < 0 || newHead.x >= BOARD_W || newHead.y < 0 || newHead.y >= BOARD_H);
  // Collision with self
  bool hitSelf = false;
  for (int i = 0; i < snakeLength; i++) {
    if (pointEquals(newHead, snake[i])) {
      hitSelf = true;
      break;
    }
  }
  if (hitWall || hitSelf) {
    gameOver = true;
    return;
  }
  // Check if food eaten
  bool ateFood = pointEquals(newHead, food);


  // Shift body right by one
  int oldLength = snakeLength;
  if (ateFood && snakeLength < MAX_SNAKE_LEN) {
    snakeLength++;
  }
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0] = newHead;
  if (ateFood) {
    if (snakeLength < MAX_SNAKE_LEN) {
      food = generateFood();
    }
  }
}


void showStartScreen() {
  drawCenteredText("Press any button", "to start", "Snake");
}


void showGameOverScreen() {
  drawCenteredText("You lost!", "Press any button", "to restart");
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  // Set I2C pins for Pico
  Wire.setSDA(OLED_SDA_PIN);
  Wire.setSCL(OLED_SCL_PIN);
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (true) {
      delay(1000);
    }
  }


  display.clearDisplay();
  display.display();
  randomSeed(analogRead(A0));
  showStartScreen();
}


void loop() {
  if (!gameStarted) {
    if (anyButtonPressed()) {
      waitForButtonRelease();
      startNewGame();
      lastLeftState = HIGH;
      lastRightState = HIGH;
    }
    return;
  }


  if (!gameOver) {
    handleTurnInput();
    unsigned long now = millis();
    if (now - lastMoveTime >= MOVE_INTERVAL_MS) {
      lastMoveTime = now;
      moveSnake();
      if (gameOver) {
        showGameOverScreen();
        delay(3000);
        waitForButtonRelease();
      } else {
        drawGame();
      }
    }
  } else {
    if (anyButtonPressed()) {
      waitForButtonRelease();
      startNewGame();
    }
  }
}
