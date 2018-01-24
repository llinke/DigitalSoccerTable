// Uncomment to enable printing debug messages.
#define DST_DEBUG
#define DEBUG_PRINTER Serial
#ifdef DST_DEBUG
#define DEBUG_PRINT(...)              \
  {                                   \
    DEBUG_PRINTER.print(__VA_ARGS__); \
  }
#define DEBUG_PRINTLN(...)              \
  {                                     \
    DEBUG_PRINTER.println(__VA_ARGS__); \
  }
#else
#define DEBUG_PRINT(...) \
  {                      \
  }
#define DEBUG_PRINTLN(...) \
  {                        \
  }
#endif

//#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "settings.h"

//#include <ArduinoOTA.h>
//#include <ESP8266mDNS.h>
#include <time.h>
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>
//#include <WiFiManager.h>

//#include "ThingspeakClient.h"

//#define PAUSE_AFTER_GOAL

#define BUTTONS_ON_I2C

// **************************************************
// *** Constants Definitions
// **************************************************
#pragma region Constants Definitions
const int buttonLockDuration = 200;      // ignore buttons for X ms, prevent prelling
const int resetGameAfter = 1000 * 2;     // MainButton pressed for X sec resets game
const int lockedGoalDuration = 1000 + 2; // ignore goal buttons / sensors for X sec
const int gameTimeAbsMax = 60 * 10;      // max X min
const int gameTimeAbsMin = 60 * 3;       // min X min
const int gameTimeDefault = 60 * 5;      // default X min
#pragma endregion

// **************************************************
// *** Variable Declarations
// **************************************************
#pragma region Variable Declarations
// [Common Variables]
volatile bool buttonPressedOnI2C = false;
volatile bool updateOledRequired = false;

// [Game Control]
volatile bool gameRunning = false;
volatile bool gamePaused = false;
volatile bool mainButtonReleased = false;
volatile uint32_t mainButtonAt = 0;
volatile bool settingsButtonPressed = false;
volatile uint32_t settingsButtonAt = 0;

// [Goals / Standings]
volatile bool wasGoal = false;
volatile int wasGoalForTeam = -1;
volatile int goals[] = {0, 0};
volatile bool lockedGoalTriggers = false;
volatile uint32_t lockedGoalTriggersAt = 0;

// [Tickers]
Ticker tickerGametime;
volatile bool gameTimeChanged = false;
int gameTimeMax = gameTimeDefault;
int gameTimeRemain = gameTimeDefault;
int gameTimeProgress = 100;
#pragma endregion

// **************************************************
// *** Application Setup
// **************************************************
#pragma region Application Setup
void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Attach ticker for game time
  tickerGametime.attach(1, onTickerGametime);

  // Initialize OLED display
  display.init();
  //display.flipScreenVertically();
  drawDisplay();

  // Wire buttons and events (I2C or directly connected)
#ifdef BUTTONS_ON_I2C
  DEBUG_PRINT("PCF8574: setting bus speed to ");
  DEBUG_PRINT(I2C_BUS_SPEED);
  DEBUG_PRINTLN(".");
  Wire.setClock(I2C_BUS_SPEED);

  DEBUG_PRINTLN("PCF8574: setting PINs.");
  expander.pinMode(0, INPUT_PULLUP);
  expander.pinMode(1, INPUT_PULLUP);
  expander.pinMode(2, INPUT_PULLUP);
  expander.pinMode(3, INPUT_PULLUP);
  expander.pinMode(4, INPUT_PULLUP);
  expander.pinMode(5, INPUT_PULLUP);
  DEBUG_PRINT("PCF8574: use I2C address ");
  DEBUG_PRINT(I2C_EXPANDER_ADDRESS);
  DEBUG_PRINTLN(".");

  DEBUG_PRINTLN("PCF8574: attaching ISR handler.");
  //expander.enableInterrupt(I2C_INT_PIN, ISRgateway);
  pinMode(I2C_INT_PIN, INPUT_PULLUP);
  attachInterrupt(I2C_INT_PIN, ISRgateway, FALLING);

  DEBUG_PRINTLN("PCF8574: attaching PIN handler.");
  /* Attach a software interrupt on pin 3 of the PCF8574 */
  expander.attachInterrupt(0, onGoalButtonTeam1, FALLING);
  expander.attachInterrupt(1, onGoalSensorTeam1, RISING);
  expander.attachInterrupt(2, onGoalSensorTeam1, RISING);
  expander.attachInterrupt(3, onGoalButtonTeam2, FALLING);
  expander.attachInterrupt(4, onGoalSensorTeam2, RISING);
  expander.attachInterrupt(5, onGoalSensorTeam2, RISING);

  DEBUG_PRINTLN("PCF8574: start listening.");
  expander.begin(I2C_EXPANDER_ADDRESS);
#else
  DEBUG_PRINTLN("Attaching direct handlers for goal buttons.");
  pinMode(BUTTON_TEAM_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_TEAM_2_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_TEAM_1_PIN, onGoalButtonTeam1, FALLING);
  attachInterrupt(BUTTON_TEAM_2_PIN, onGoalButtonTeam2, FALLING);
#endif

  pinMode(BUTTON_MAIN_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_MAIN_PIN, onMainButton, CHANGE);

  pinMode(BUTTON_SETTINGS_PIN, INPUT);
  attachInterrupt(BUTTON_SETTINGS_PIN, onSettingsButton, RISING);
}
#pragma endregion

// **************************************************
// *** Application Loop
// **************************************************
#pragma region Application Loop
void loop()
{
#ifdef BUTTONS_ON_I2C
  // Evaluate expander pins and execute attached callbacks
  if (buttonPressedOnI2C)
  {
    buttonPressedOnI2C = false;
    DEBUG_PRINTLN("PCF8574: change of buttons' state detected in ISRgateway.");
    expander.checkForInterrupt();
  }
#endif

  // Handle main and settings buttons
  if (mainButtonReleased)
  {
    handleMainButton();
  }
  if (settingsButtonPressed)
  {
    handleSettingsButton();
  }

  // Update remaining game time if timer was triggered
  if (gameTimeChanged)
  {
    updateGameTime(-1);
  }

  // Handle goal detected
  if (wasGoal)
  {
    updateGoalStats();
  }
  if (lockedGoalTriggers &&
      ((millis() - lockedGoalTriggersAt) > lockedGoalDuration))
  {
    // Release lock after X ms
    lockedGoalTriggers = false;
    gamePaused = false;
    updateOledRequired = true;
    DEBUG_PRINTLN("Goal lock released.");
  }

  // Update OLED display only if required
  if (updateOledRequired)
  {
    drawDisplay();
    updateOledRequired = false;
  }

  // REQUIRED: allow processing of interrupts etc
  delay(0);
  //yield();
}
#pragma endregion

// **************************************************
// *** Event Handlers (Buttons, Tickers)
// **************************************************
#pragma region Event Handlers
// [I2C Expander Interrupt Service Routine]
#ifdef BUTTONS_ON_I2C
void ISRgateway()
{
  //os_intr_lock();
  buttonPressedOnI2C = true;
  //expander.checkForInterrupt();
  //DEBUG_PRINTLN("PCF8574: change of buttons' state detected in ISRgateway.");
  //os_intr_unlock();
}
#endif

// [Goal Buttons/Sensors]
void onGoalButtonTeam1()
{
  goalScoredByTeam(0);
}
void onGoalSensorTeam1()
{
  goalScoredByTeam(0);
}
void onGoalButtonTeam2()
{
  goalScoredByTeam(1);
}
void onGoalSensorTeam2()
{
  goalScoredByTeam(1);
}
void goalScoredByTeam(int team)
{
  if (!gameRunning)
    return;
  if (gamePaused)
    return;
  if (lockedGoalTriggers)
    return; // ignore, triggers still locked
  if (wasGoal)
    return; // ignore, already triggered

  /*
  DEBUG_PRINT("Goal scored by team ");
  DEBUG_PRINT(team + 1);
  DEBUG_PRINTLN(".");
  */

  wasGoalForTeam = team;
  wasGoal = true;
}

// [Main Button]
void onMainButton()
{
  int val = digitalRead(BUTTON_MAIN_PIN);
  if (val == 0)
  {
    //DEBUG_PRINTLN("Button MAIN pressed.");
    mainButtonAt = millis();
    mainButtonReleased = false;
  }
  else
  {
    if (mainButtonReleased)
      return; // ignore, already triggered

    uint32_t pressedFor = millis() - mainButtonAt;
    /*
    DEBUG_PRINT("Button MAIN released, pressed for ");
    DEBUG_PRINT(pressedFor);
    DEBUG_PRINTLN("ms.");
    */
    if (pressedFor < buttonLockDuration)
    {
      //DEBUG_PRINTLN("IGNORED!");
      return; // ignore if press detected again during lock duration
    }

    mainButtonReleased = true;
  }
}

void handleMainButton()
{
  uint32_t pressedFor = millis() - mainButtonAt;
  DEBUG_PRINT("Button MAIN released, pressed for ");
  DEBUG_PRINT(pressedFor);
  DEBUG_PRINT("ms: ");

  if (pressedFor < resetGameAfter)
  {
    gameRunning = !gameRunning;
    updateOledRequired = true;
    DEBUG_PRINTLN(gameRunning ? "game started." : "games paused.");
  }
  else
  {
    gameRunning = false;
    DEBUG_PRINTLN("resetting game.");
    resetGame();
  }

  mainButtonReleased = false;
}

// [Settings Button (Game Time)]
void onSettingsButton()
{
  if (gameRunning)
    return; // ignore if game is running
  if (settingsButtonPressed)
    return; // ignore, already triggered

  settingsButtonPressed = true;
}

void handleSettingsButton()
{
  if (gameRunning)
    return; // ignore if game is running

  uint32_t pressedAgo = settingsButtonAt > 0 ? millis() - settingsButtonAt : buttonLockDuration;
  DEBUG_PRINT("Button SETTINGS pressed ");
  DEBUG_PRINT(pressedAgo);
  DEBUG_PRINTLN("ms ago.");
  if (pressedAgo < buttonLockDuration)
  {
    DEBUG_PRINTLN("IGNORED!");
  }
  else
  {
    gameTimeMax += 60;
    if (gameTimeMax > gameTimeAbsMax)
      gameTimeMax = gameTimeAbsMin;
    gameTimeRemain = gameTimeMax;

    DEBUG_PRINT("Changed game time to ");
    DEBUG_PRINT(gameTimeMax);
    DEBUG_PRINTLN("sec.");

    settingsButtonAt = millis();
    updateGameTime(0);
  }

  settingsButtonPressed = false;
}

// [Game Time Ticker]
void onTickerGametime()
{
  if (!gameRunning)
    return;
  if (gamePaused)
    return;

  gameTimeChanged = true;
}
#pragma endregion

// **************************************************
// *** Helper Methods
// **************************************************
#pragma region Helper Methods
// [Game Controls]
void updateGoalStats()
{
  if (!gameRunning)
    return;
  if (gamePaused)
    return;
  if (lockedGoalTriggers)
    return;
  if (!wasGoal)
    return;

  goals[wasGoalForTeam]++;
  lockedGoalTriggers = true;
  lockedGoalTriggersAt = millis();
  gamePaused = true;
#ifdef PAUSE_AFTER_GOAL
  gameRunning = false;
#endif
  wasGoal = false;
  wasGoalForTeam = -1;

  DEBUG_PRINT("Goal scored for team ");
  DEBUG_PRINT(wasGoalForTeam + 1);
  DEBUG_PRINT(", standing: ");
  DEBUG_PRINT(goals[0]);
  DEBUG_PRINT(":");
  DEBUG_PRINT(goals[1]);
  DEBUG_PRINTLN(".");

  // Require update of OLED display
  updateOledRequired = true;
}

void updateGameTime(int decBy)
{
  if (gameTimeRemain > 0)
  {
    gameTimeRemain += decBy;
  }
  /*
  if (gameTimeRemain < 0)
  {
    gameTimeRemain = gameTimeMax;
  }
  */
  gameTimeProgress = (gameTimeRemain * 100) / gameTimeMax;
  DEBUG_PRINT("Remaining time: ");
  DEBUG_PRINT(gameTimeRemain);
  DEBUG_PRINT(", in %: ");
  DEBUG_PRINT(gameTimeProgress);
  DEBUG_PRINTLN(".");
  gameTimeChanged = false;

  // Require update of OLED display
  updateOledRequired = true;
}

void resetGame()
{
  // Reset game settings
  gameRunning = false;
  gamePaused = false;
  goals[0] = 0;
  goals[1] = 0;
  wasGoal = false;
  wasGoalForTeam = -1;
  lockedGoalTriggers = false;
  lockedGoalTriggersAt = millis();

  // Reset time settings
  gameTimeRemain = gameTimeMax;
  updateGameTime(0);

  // Reset OLED display
  updateOledRequired = true;
}

// [Update OLED Display]
void drawDisplay()
{
  // clear the display
  display.clear();

  display.setColor(OLEDDISPLAY_COLOR::WHITE);
  display.setFont(Dialog_plain_12);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  //display.drawString(64, 0, ":");
  display.drawString(32, 0, "HEIM");
  display.drawString(96, 0, "GAST");

  display.setFont(Nimbus_Sans_L_Regular_Condensed_32);
  char goals_str[3];
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 12, ":");
  //sprintf(goals_str, "%02d", goals[0]);
  sprintf(goals_str, "%d\n", goals[0]);
  display.drawString(32, 12, goals_str);
  //sprintf(goals_str, "%02d", goals[1]);
  sprintf(goals_str, "%d\n", goals[1]);
  display.drawString(96, 12, goals_str);

  int progbar = (gameTimeProgress * 128) / 100;
  display.drawRect(0, 50, 128, 14);
  if (gameRunning && !gamePaused)
  {
    display.fillRect(0, 50, progbar, 14);
  }
  display.setFont(Dialog_plain_12);
  char time_str[6];
  int time_s = gameTimeRemain % 60;
  int time_m = (gameTimeRemain - time_s) / 60;
  sprintf(time_str, "%02d:%02d\n", time_m, time_s);
  if (gameRunning && !gamePaused)
  {
    if (gameTimeProgress < 50)
    {
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setColor(OLEDDISPLAY_COLOR::WHITE);
      display.drawString(progbar + 2, 50, time_str);
    }
    else
    {
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.setColor(OLEDDISPLAY_COLOR::BLACK);
      display.drawString(progbar - 2, 50, time_str);
    }
  }
  else
  {
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setColor(OLEDDISPLAY_COLOR::WHITE);
    display.drawString(64, 50, time_str);
  }

  /*
  display.setColor(OLEDDISPLAY_COLOR::WHITE);
  display.drawVerticalLine(64, 0, 50);
  display.drawHorizontalLine(0, 14, 128);
  */
  display.setColor(OLEDDISPLAY_COLOR::INVERSE);
  display.fillRect(0, 0, 128, 14);
  display.setColor(OLEDDISPLAY_COLOR::WHITE);
  display.drawRect(0, 0, 128, 64);

  // write the buffer to the display
  display.display();
}
#pragma endregion
