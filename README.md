# Snake Game on Raspberry Pi Pico 2 W with SSD1306 OLED

## Overview
This project is a hardware–software integrated Snake game implemented on a **Raspberry Pi Pico 2 W** using the **Arduino IDE**. The game is displayed on a **128×64 SSD1306 I2C OLED screen** and controlled with **two push buttons**.

The program demonstrates:
- embedded firmware development
- sensor/input handling
- OLED display control over I2C
- real-time game logic
- structured separation between input, storage, processing, and output

-----------

## Features
- Snake game rendered on a 128×64 OLED
- Two-button control:
  - left button = turn left
  - right button = turn right
- Rotation-matrix-based turning logic
- Random food generation
- Wall collision detection
- Self-collision detection
- Start screen and game over screen
- Restart after losing

-------

## Hardware Used
- Raspberry Pi Pico 2 W
- SSD1306 128×64 I2C OLED display
- 2 push buttons
- jumper wires
- breadboard

--------

## Pin Connections

### OLED Display
- **OLED SDA** -> **GP4**
- **OLED SCL** -> **GP5**
- **OLED VCC** -> **3.3V**
- **OLED GND** -> **GND**


### Buttons
- **Left button** → **GP0**
- **Right button** → **GP1**
- Other side of each button → **GND**

The buttons are configured with `INPUT_PULLUP`, so:
- not pressed = `HIGH`
- pressed = `LOW`
------------------------------

## Libraries Required
Install these libraries in the Arduino IDE:
- **Wire**
- **Adafruit GFX Library**
- **Adafruit SSD1306**

------------------------

## Board Configuration
In Arduino IDE, select the correct board for the Pico 2 W.

Typical setup:
- Board: **Raspberry Pi Pico 2 W**
- Port: your connected Pico serial port

---------------------

## Program Structure

### 1. Input Layer
Reads button presses from:
- `LEFT_BUTTON_PIN`
- `RIGHT_BUTTON_PIN`

The program uses edge detection so one press causes one turn.

### 2. Storage Layer
Stores game state in variables such as:
- `snake[]`
- `snakeLength`
- `food`
- `dir`
- `gameStarted`
- `gameOver`

### 3. Processing Layer
Handles:
- new game setup
- direction changes
- snake movement
- collision detection
- food generation
- timing control

### 4. Output Layer
Uses the SSD1306 OLED to display:
- start screen
- live snake game
- game over screen

---

## Important Variables

### Display Configuration
- `SCREEN_WIDTH` = 128
- `SCREEN_HEIGHT` = 64
- `OLED_ADDR` = `0x3C`

### Board Geometry
- `CELL_SIZE` = 8
- `BOARD_W` = 16
- `BOARD_H` = 8

This divides the display into a **16×8 grid**.

### Timing
- `MOVE_INTERVAL_MS` = 200

The snake moves every 200 ms.

---

## How the Snake Movement Works
The snake direction is stored as a 2D vector:
- right = `(1, 0)`
- left = `(-1, 0)`
- down = `(0, 1)`
- up = `(0, -1)`

Turning uses rotation matrices :)

### Left Turn
[ 0  1 ]  
[-1  0 ]

### Right Turn
[ 0 -1 ]  
[ 1  0 ]

This allows the snake to rotate cleanly without long chains of `if` statements.

--

## Main Functions

### startNewGame()
Initializes the snake in the center of the board, resets flags, generates food, and starts the timer.

### rotateLeft() / rotateRight()
Rotates the movement vector using a rotation matrix.

### generateFood()
Places food randomly in a grid cell that is not occupied by the snake.

### drawCenteredText()
Displays centered text for title and game-over screens.

### drawGame()
Draws:
- snake head
- snake body
- food

### handleTurnInput()
Reads button input and changes direction on a new button press.

### moveSnake()
Moves the snake, checks:
- wall collision
- self collision
- food collision

### showStartScreen()
Displays the start message.

### showGameOverScreen()
Displays the game-over message.

### setup()
Initializes:
- serial
- button pin modes
- I2C
- OLED
- random seed

### loop()
Main control loop:
- waits for game start
- reads input
- updates movement on a timed interval
- redraws the screen
- handles restart after game over

--

## How to Use the arduino and get snake game to start
1. Power the Pico 2 W.
2. The OLED will show the start screen.
3. Press either button to begin.
4. Use:
   - left button to rotate left
   - right button to rotate right
5. Eat food to grow.
6. Avoid:
   - hitting walls
   - hitting the snake’s own body
7. After losing, press a button to restart.

--

## Notes
- The OLED uses I2C communication
- The display address is usually `0x3C`, though some modules may use `0x3D`
- The buttons are active-low because of `INPUT_PULLUP`
- The game grid is based on 8×8 cells to fit neatly on a 128×64 screen

--

## Possible Improvements
- add a pause menu
- add a leaderboard
- save high scores in non-volatile memory
- add sound or haptic feedback
- add touch input
- add communication with a Java application over serial

## Team members:
Maximus Angelini, Eric Xiao, Kevin Yu, Ian he
