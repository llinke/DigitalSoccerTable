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

volatile bool gameRunning = false;
volatile uint32_t mainButtonAt = 0;
volatile uint32_t settingsButtonAt = 0;
const int buttonLockDuration = 100;
const int mbResetAfter = 3000;

volatile bool buttonPressed = false;

int goals[] = {0, 0};
volatile bool wasGoal = false;
volatile uint32_t wasGoalAt = 0;
volatile int lastGoalForTeam = -1;
const int gbLockDuration = 1000;

Ticker tickerGametime;
const int gameTimeAbsMax = 60 * 10; // max 10min
const int gameTimeAbsMin = 60 * 3;  // min 3min
const int gameTimeDefault = 60 * 5; // default 5min
int gameTimeMax = gameTimeDefault;
int gameTimeRemain = gameTimeDefault;
int progress = 100;
volatile bool progressChanged = true;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  tickerGametime.attach(1, onTickerGametime);

  display.init();
  //display.flipScreenVertically();
  drawDisplay();

#ifdef BUTTONS_ON_I2C
  Serial.print("PCF8574: setting bus speed to ");
  Serial.print(I2C_BUS_SPEED);
  Serial.println(".");
  Wire.setClock(I2C_BUS_SPEED);

  Serial.println("PCF8574: setting PINs.");
  expander.pinMode(0, INPUT_PULLUP);
  expander.pinMode(1, INPUT_PULLUP);
  expander.pinMode(2, INPUT_PULLUP);
  expander.pinMode(3, INPUT_PULLUP);
  expander.pinMode(4, INPUT_PULLUP);
  expander.pinMode(5, INPUT_PULLUP);
  Serial.print("PCF8574: use I2C address ");
  Serial.print(I2C_EXPANDER_ADDRESS);
  Serial.println(".");

  Serial.println("PCF8574: attaching ISR handler.");
  //expander.enableInterrupt(I2C_INT_PIN, ISRgateway);
  pinMode(I2C_INT_PIN, INPUT_PULLUP);
  attachInterrupt(I2C_INT_PIN, ISRgateway, FALLING);

  Serial.println("PCF8574: attaching PIN handler.");
  /* Attach a software interrupt on pin 3 of the PCF8574 */
  expander.attachInterrupt(0, goalButtonTeam1, FALLING);
  expander.attachInterrupt(1, goalSensorTeam1, RISING);
  expander.attachInterrupt(2, goalSensorTeam1, RISING);
  expander.attachInterrupt(3, goalButtonTeam2, FALLING);
  expander.attachInterrupt(4, goalSensorTeam2, RISING);
  expander.attachInterrupt(5, goalSensorTeam2, RISING);

  Serial.println("PCF8574: start listening.");
  expander.begin(I2C_EXPANDER_ADDRESS);
#else
  Serial.println("Attaching direct handlers for goal buttons.");
  pinMode(BUTTON_TEAM_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_TEAM_2_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_TEAM_1_PIN, goalButtonTeam1, FALLING);
  attachInterrupt(BUTTON_TEAM_2_PIN, goalButtonTeam2, FALLING);
#endif

  pinMode(BUTTON_MAIN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SETTINGS_PIN, INPUT);
  attachInterrupt(BUTTON_MAIN_PIN, mainButtonChanged, CHANGE);
  attachInterrupt(BUTTON_SETTINGS_PIN, settingsButton, CHANGE);
}

void loop()
{
  if (wasGoal || progressChanged)
  {
    drawDisplay();
    progressChanged = false;
  }
  if (wasGoal && (wasGoalAt + gbLockDuration < millis()))
  {
    wasGoal = false;
  }

#ifdef BUTTONS_ON_I2C
  if (buttonPressed)
  {
    Serial.println("PCF8574: change of buttons' state handled.");
    buttonPressed = false;
  }
/*
  else
  {
    uint8_t intPin = digitalRead(I2C_INT_PIN);
    if (intPin == 0) // && !buttonPressed)
    {
      Serial.print("PCF8574: I2C_INT_PIN is ");
      Serial.print(intPin);
      Serial.println(", manually checking for updates...");
      expander.checkForInterrupt();
      dumpI2CPins();
    }
  }
*/
#endif

  delay(0);
  yield();
}

void ISRgateway()
{
  if (false) //wasGoal)
  {
    expander.read(); // Read to reset INT trigger
    buttonPressed = false;
    //Serial.println("PCF8574: change of buttons' state IGNORED in ISRgateway.");
  }
  else
  {
    expander.checkForInterrupt();
    buttonPressed = true;
    Serial.println("PCF8574: change of buttons' state detected in ISRgateway.");
  }
}

#ifdef BUTTONS_ON_I2C
void dumpI2CPins()
{
  Serial.print("PCF8574: ");
  int digRead = 0;
  for (int p = 0; p < 8; p++)
  {
    digRead = expander.digitalRead(p);
    Serial.print(digRead);
    Serial.print("|");
  }
  Serial.println(".");
}
#endif

void goalButtonTeam1()
{
  Serial.println("Button TEAM 1 pressed.");
  goalScoredFor(0);
}
void goalSensorTeam1()
{
  Serial.println("Sensor TEAM 1 triggered.");
  goalScoredFor(0);
}

void goalButtonTeam2()
{
  Serial.println("Button TEAM 2 pressed.");
  goalScoredFor(1);
}
void goalSensorTeam2()
{
  Serial.println("Sensor TEAM 2 triggered.");
  goalScoredFor(1);
}

void goalScoredFor(int team)
{
  if (!gameRunning)
    return;
  if (wasGoal)
    return;
  goals[team]++;
  lastGoalForTeam = team;
  wasGoal = true;
  wasGoalAt = millis();
  Serial.print("Goal scored for team ");
  Serial.print(team + 1);
  Serial.print(", standing: ");
  Serial.print(goals[0]);
  Serial.print(":");
  Serial.print(goals[1]);
  Serial.println(".");
#ifdef PAUSE_AFTER_GOAL
  gameRunning = false;
#endif
}

void resetGame()
{
  gameRunning = false;
  goals[0] = 0;
  goals[1] = 0;
  wasGoal = true;
  wasGoalAt = millis();
  lastGoalForTeam = -1;
  gameTimeRemain = gameTimeMax;
  updateGameTime();
}

void mainButtonChanged()
{
  int val = digitalRead(BUTTON_MAIN_PIN);
  if (val == 0)
  {
    Serial.println("Button MAIN pressed.");
    mainButtonAt = millis();
  }
  else
  {
    if (mainButtonAt == 0)
      mainButtonAt = millis();
    uint32_t pressedFor = millis() - mainButtonAt;
    Serial.print("Button MAIN released, pressed for ");
    Serial.print(pressedFor);
    Serial.print("ms: ");
    if (pressedFor < buttonLockDuration)
    {
      Serial.println("ignored.");
      // ignore, probably prelled
      mainButtonAt = millis();
      return;
    }
    else if (pressedFor < mbResetAfter)
    {
      gameRunning = !gameRunning;
      Serial.println(gameRunning ? "game started." : "games paused.");
      return;
    }
    else
    {
      Serial.println("resetting game.");
      resetGame();
      return;
    }
  }
}

void settingsButton()
{
  if (gameRunning)
    return;
  int val = digitalRead(BUTTON_SETTINGS_PIN);
  if (val == 1)
  {
    Serial.println("Button SETTINGS pressed.");
    mainButtonAt = millis();
  }
  else
  {
    if (settingsButtonAt == 0)
      settingsButtonAt = millis();
    uint32_t pressedFor = millis() - settingsButtonAt;
    Serial.print("Button SETTINGS release, was pressed for ");
    Serial.print(pressedFor);
    Serial.print("ms, ");
    if (pressedFor < buttonLockDuration)
    {
      Serial.println("ignored.");
      // ignore, probably prelled
      settingsButtonAt = millis();
      return;
    }
    else
    {
      settingsButtonAt = millis();
      gameTimeMax += 60;
      if (gameTimeMax > gameTimeAbsMax)
        gameTimeMax = gameTimeAbsMin;
      Serial.print("changing game time to ");
      Serial.print(gameTimeMax);
      Serial.println("sec.");
      gameTimeRemain = gameTimeMax;
      updateGameTime();
    }
  }
}

void onTickerGametime()
{
  if (!gameRunning)
    return;
  gameTimeRemain--;
  if (gameTimeRemain < 0)
  {
    gameTimeRemain = gameTimeMax;
  }
  updateGameTime();
}
void updateGameTime()
{
  progress = (gameTimeRemain * 100) / gameTimeMax;
  Serial.print("Remaining time: ");
  Serial.print(gameTimeRemain);
  Serial.print(", progress: ");
  Serial.print(progress);
  Serial.println(".");
  progressChanged = true;
}

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

  int progbar = (progress * 128) / 100;
  display.drawRect(0, 50, 128, 14);
  display.fillRect(0, 50, progbar, 14);
  display.setFont(Dialog_plain_12);
  char time_str[6];
  int time_s = gameTimeRemain % 60;
  int time_m = (gameTimeRemain - time_s) / 60;
  sprintf(time_str, "%02d:%02d\n", time_m, time_s);
  if (progress < 50)
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