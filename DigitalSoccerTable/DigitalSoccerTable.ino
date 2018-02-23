// **************************************************
// *** Compiler Flags
// **************************************************
// --- WiFi -----------------------------------------
//#define INCLUDE_WIFI
// --- Demo --------- -------------------------------
#define PLAY_DEMO true
#define COMET_DEMO
// --- DEBUG ----------------------------------------
#define DST_DEBUG
//#define DEBUG_LOOP
// --- Buttons --------------------------------------
#define BUTTONS_ON_I2C
#define SENSORS_ON_I2C
#define BUTTONS_MAIN_CFG_ON_I2C
#ifndef BUTTONS_MAIN_CFG_ON_I2C
#define BUTTON_TEAM_1_PIN D5
#define BUTTON_TEAM_2_PIN D6
#define BUTTON_MAIN_PIN D7
#define BUTTON_SETTINGS_PIN D8
#endif
// --- Game Control----------------------------------
//#define PAUSE_AFTER_GOAL
// **************************************************

// **************************************************
// *** Includes
// **************************************************
#include "SerialDebug.h"
#include "I2CInclude.h"
#include "FastLedInclude.h"
#include "ColorPalettes.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
#include <map>
#include <string.h>
#include "NeoGroup.cpp"

#ifdef INCLUDE_WIFI
#include <ESP8266WiFi.h>	  //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>		  //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>	  //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
//#include <ArduinoOTA.h>
//#include <ESP8266mDNS.h>
#endif

#include <Ticker.h>
#include <time.h>
// **************************************************

// **************************************************
// *** Constants Definitions
// **************************************************
#pragma region Constants Definitions
const int buttonLockDuration = 200;		 // ignore buttons for X ms, prevent prelling
const int resetGameAfter = 1000 * 2;	 // MainButton pressed for X sec resets game
const int lockedGoalDuration = 1000 + 2; // ignore goal buttons / sensors for X sec
const int gameTimeAbsMax = 60 * 10;		 // max X min
const int gameTimeAbsMin = 60 * 3;		 // min X min
const int gameTimeDefault = 60 * 5;		 // default X min

const uint8_t globalBrightness = 128;
// 0: Wave, 1: Dynamic Wave, 2: Noise, 3: Confetti, 4: Fade, 5: Comet
const int maxFxNr = 5;
const int defaultFxNr = 1;
const int defaultFxNrAll = 5;
const String defaultColPal = "Idle";
const String defaultColPalAll = "Rainbow";
const int defaultFps = 50;	//25;
const int defaultGlitter = 0; //32;
#pragma endregion

// **************************************************
// *** Variable Declarations
// **************************************************
#pragma region Variable Declarations
// [Common Variables]
volatile bool buttonPressedOnI2C = false;
volatile bool updateOledRequired = false;

// [Game Control]
String teamName1 = "HEIM"; //"Dortmund";
String teamName2 = "GAST"; //"Bayern";
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

// [LED Strip]
// Static size
struct CRGB leds[PIXEL_COUNT];
// Dynamic size:
//struct CRGB *leds = NULL;
bool ledsInitialized = false;
bool ledsStarted = false;
int maxColNr = 1; // will be dynamically assigned once palettes are generated
volatile int changeColorForTeam = -1;

//std::vector<NeoGroup *> neoGroups;
std::vector<NeoGroup> neoGroups;
#pragma endregion

// **************************************************
// *** WiFi
// **************************************************
#pragma region WiFi
#ifdef INCLUDE_WIFI
bool InitWifi(bool useWifiCfgTimeout = true, bool forceReconnect = false)
{
	DEBUG_PRINTLN("WIFI ------------------------------------------------------");
	if (!forceReconnect && WiFi.status() == WL_CONNECTED)
	{
		DEBUG_PRINTLN("WiFi: already connected...");
		return true; // Is already connected...
	}

	if (ledsInitialized)
	{
		FastLED.clear(true);
		fill_solid(leds, PIXEL_COUNT, CRGB::Blue);
		FastLED.show();
	}
	delay(2500);
	//WiFiManager
	WiFiManager wifiManager;
	if (forceReconnect)
	{
		wifiManager.resetSettings();
	}
	//wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
	//fetches ssid and pass from eeprom and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP" and goes into a blocking loop awaiting configuration
	DEBUG_PRINTLN("WiFi Manager trying to connect...");
	if (useWifiCfgTimeout)
	{
		DEBUG_PRINT("You have ");
		DEBUG_PRINT(ConfigureAPTimeout);
		DEBUG_PRINTLN(" seconds for configuration if required.");
		wifiManager.setConfigPortalTimeout(ConfigureAPTimeout);
	}
	bool connected = wifiManager.autoConnect(wifiApName.c_str());
	//or use this for auto generated name ESP + ChipID
	//wifiManager.autoConnect();
	//if you get here you have connected to the WiFi
	if (ledsInitialized)
	{
		fill_solid(leds, PIXEL_COUNT, connected ? CRGB::Green : CRGB::Red);
		FastLED.show();
	}
	if (connected)
	{
		DEBUG_PRINTLN("Wifi is connected...yay!!!");
	}
	else
	{
		DEBUG_PRINTLN("!!! WIFI NOT CONNECTED !!!");
	}
	delay(5000);

	return connected;
}
#endif
#pragma endregion

// **************************************************
// *** LED Strip
// **************************************************
#pragma region LED Strip
// [FastLED Helper methods]
int initStrip(bool doStart = false, bool playDemo = true)
{
	if (ledsInitialized)
	{
		return doStart ? startStrip() : PIXEL_COUNT;
	}

	DEBUG_PRINTLN("LEDStrip --------------------------------------------------");
	/*	
	DEBUG_PRINTLN("Allocating memory for LED strip data.");
	leds = (struct CRGB *)malloc(PIXEL_COUNT * sizeof(struct CRGB));
	*/
	DEBUG_PRINTLN("Assigning LEDs to FastLED.");
	FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN>(leds, PIXEL_COUNT);
	//FastLED.setMaxPowerInVoltsAndMilliamps(5,3000);
	FastLED.setBrightness(globalBrightness);
	FastLED.setDither(0);
	FastLED.clear(true);
	FastLED.show();

	ledsInitialized = true;

	if (playDemo)
	{
		DEBUG_PRINT("Playing demo effect...");
#ifdef COMET_DEMO
		CRGBPalette16 colorPalette = NeoGroup::GenerateRGBPalette(CommonColorPalettes.find("Rainbow")->second);
		for (int dot = 0; dot < 256; dot++)
		{
			fadeToBlackBy(leds, PIXEL_COUNT, 8);
			int variant = (PIXEL_COUNT / 16);
			int pos = ease8InOutQuad(dot) + random(0 - variant, 0 + variant);
			pos = (pos * PIXEL_COUNT) / 256;
			pos = constrain(pos, 0, PIXEL_COUNT);
			//DEBUG_PRINT("Setting pixel #");
			//DEBUG_PRINTLN(pos);
			int bright = random(64, 255);

			/*
			CRGB color = CHSV(random8(), 255, 255);
			nblend(leds[pos], color, 128);
			*/
			uint8_t colpos = dot + random8(16) - 8;
			nblend(leds[pos], ColorFromPalette(colorPalette, colpos, bright), 128);

			FastLED.show();
			delay(10);
			DEBUG_PRINT(".");
		}
		DEBUG_PRINTLN("DONE");
#else
		for (int dot = 0; dot < PIXEL_COUNT; dot++)
		{
			leds[dot] = CHSV(random8(), 255, 255);
			FastLED.show();
			delay(10);
			DEBUG_PRINT(".");
		}
		DEBUG_PRINTLN("DONE");
		delay(500);
#endif

		DEBUG_PRINTLN("Fading away demo effect.");
		for (int fade = 0; fade < 20; fade++)
		{
			fadeToBlackBy(leds, PIXEL_COUNT, 20);
			FastLED.show();
			delay(50);
		}
		DEBUG_PRINTLN("Clearing LEDs.");
		FastLED.clear(true);
		FastLED.show();
	}

#ifdef INCLUDE_WIFI
	InitWifi();
#endif

	DEBUG_PRINTLN("Adding special groups.");
	neoGroups.clear();
	// Group 0: all LEDs
	addGroup("All LEDs' group", 0, PIXEL_COUNT, 0);
	addGroup("Team 1 group", 0, PIXEL_COUNT / 2, 0);
	addGroup("Team 2 group", PIXEL_COUNT / 2, PIXEL_COUNT / 2, 0);

	return doStart ? startStrip() : PIXEL_COUNT;
}

int startStrip()
{
	if (!ledsInitialized)
		return -1;

	ledsStarted = true;
	return PIXEL_COUNT;
}

int stopStrip()
{
	ledsStarted = false;

	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = &(neoGroups.at(i));
		neoGroup->Stop(true);
	}

	FastLED.clear(true);
	FastLED.show();

	return 0;
}

int addGroup(String grpId, int ledFirst, int ledCount, int ledOffset)
{
	if ((ledFirst >= PIXEL_COUNT) ||
		(ledCount <= 0) ||
		(ledFirst + ledCount) > PIXEL_COUNT)
		return -((((3 * 1000) + ledFirst) * 1000) + ledCount); // Invalid parameter

	DEBUG_PRINT("Adding group '");
	DEBUG_PRINT(grpId);
	DEBUG_PRINT("' at ");
	DEBUG_PRINT(ledFirst);
	DEBUG_PRINT(" with size ");
	DEBUG_PRINTLN(ledCount);

	// V1: new NeoGroup
	//NeoGroup *newGroup = new NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	//neoGroups.push_back(newGroup);
	// V2: NeoGroup w/o new
	NeoGroup newGroup = NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	neoGroups.push_back(newGroup);

	DEBUG_PRINT("GroupCount: ");
	DEBUG_PRINTLN(neoGroups.size());

	return neoGroups.size();
}

bool isGroupActive(int grpNr)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	return neoGroup->Active;
}

int startGroup(int grpNr, bool runOnlyOnce = false)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Start(runOnlyOnce);

	updateOledRequired = true;

	return grpNr;
}

int stopGroup(int grpNr, bool stopNow = false)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Stop(stopNow);

	updateOledRequired = true;

	return grpNr;
}

int setGrpEffect(
	int grpNr,
	pattern pattern,
	uint16_t length = 0,
	int amountglitter = -1,
	uint8_t fps = 0,
	direction direction = FORWARD,
	mirror mirror = MIRROR0,
	wave wave = LINEAR)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Stop();

	//int fxGlitter = amountglitter <= 0 ? neoGroup->GetGlitter() : amountglitter;
	int fxGlitter = amountglitter <= 0 ? defaultGlitter : amountglitter;
	//uint8_t fxFps = fps <= 0 ? neoGroup->GetFps() : fps;
	uint8_t fxFps = fps <= 0 ? defaultFps : fps;

	uint16_t result = neoGroup->ConfigureEffect(pattern, length, fxGlitter, fxFps, direction, mirror, wave);
	//neoGroup->Start();

	updateOledRequired = true;

	return result;
}

int setGrpColors(
	int grpNr,
	std::vector<CRGB> colors,
	bool clearFirst = true,
	bool generatePalette = true,
	bool crossFade = false)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	//neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureColors(colors, clearFirst, generatePalette, crossFade);
	//neoGroup->Start();

	updateOledRequired = true;

	return result;
}

void SetEffect(int grpNr, int fxNr, bool startFx, bool onlyOnce)
{
	DEBUG_PRINTLN("SetEffect ---------------------------------------------------");
	DEBUG_PRINT("Fx: Configuring LED effect #");
	DEBUG_PRINT(fxNr);
	DEBUG_PRINT(" for group #");
	DEBUG_PRINTLN(grpNr);

	String fxPatternName = "";
	pattern fxPattern = pattern::STATIC;
	uint16_t fxLength = 255;
	int fxGlitter = defaultGlitter;
	uint8_t fxFps = defaultFps;
	mirror fxMirror = MIRROR0;
	wave fxWave = wave::LINEAR;

	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	switch (fxNr)
	{
	case 0: // Wave
		fxPatternName = "Wave";
		fxPattern = pattern::WAVE;
		fxLength = (neoGroup->LedCount * 1.5); // 48;
		fxMirror = mirror::MIRROR2;
		break;
	case 1: // Dynamic Wave
		fxPatternName = "Dynamic Wave";
		fxPattern = pattern::DYNAMICWAVE;
		fxMirror = mirror::MIRROR1; // mirror::MIRROR0;
		break;
	case 2: // Noise
		fxPatternName = "Noise";
		fxPattern = pattern::NOISE;
		fxMirror = mirror::MIRROR1; // mirror::MIRROR0;
		fxFps *= 2;					// double FPS looks better
		break;
	case 3: // confetti
		fxPatternName = "Confetti";
		fxPattern = pattern::CONFETTI;
		fxGlitter = 0;
		fxFps /= 2; // half FPS looks better
		break;
	case 4: // Fade
		fxPatternName = "Fade";
		fxPattern = pattern::FADE;
		fxFps /= 2; // half FPS looks better
		break;
	case 5: // Comet
		fxPatternName = "Comet";
		fxPattern = pattern::COMET;
		//fxWave = wave::EASE;
		fxWave = wave::SINUS;
		fxFps *= 1.5;				// faster FPS looks better
		fxMirror = mirror::MIRROR0; //mirror::MIRROR2;
		break;
	default:
		fxPatternName = "Static";
		fxPattern = pattern::STATIC;
		fxMirror = mirror::MIRROR0;
		break;
	}
	DEBUG_PRINT("Fx: Changing effect to '");
	DEBUG_PRINT(fxPatternName);
	DEBUG_PRINTLN("'");
	setGrpEffect(
		grpNr,
		fxPattern,
		fxLength,
		fxGlitter,
		fxFps,
		direction::FORWARD,
		fxMirror,
		fxWave);
	if (startFx)
		startGroup(grpNr, onlyOnce);
}

void InitColorNames()
{
	InitColorPalettes();
}

void SetColors(int grpNr, String palKey)
{
	DEBUG_PRINTLN("SetColors --------------------------------------------------");
	DEBUG_PRINT("Col: Configuring LED colors #");
	DEBUG_PRINT(palKey);
	DEBUG_PRINT(" for group #");
	DEBUG_PRINTLN(grpNr);

	DEBUG_PRINT("Col: Changing color palette to '");
	DEBUG_PRINT(palKey);
	DEBUG_PRINT("' ");
	std::vector<CRGB> colors = {};
	if (grpNr == 0)
	{
		DEBUG_PRINTLN("of CommonColorPalettes.");
		if (CommonColorPalettes.find(palKey) != CommonColorPalettes.end())
			colors = CommonColorPalettes.find(palKey)->second;
	}
	else
	{
		DEBUG_PRINTLN("of TeamColorPalettes.");
		if (TeamColorPalettes[grpNr - 1].find(palKey) != TeamColorPalettes[grpNr - 1].end())
			colors = TeamColorPalettes[grpNr - 1].find(palKey)->second;
	}
	setGrpColors(grpNr, colors, true, true, CROSSFADE_PALETTES);
	DEBUG_PRINTLN("Dumping palette's colors...");
	DumpPalette(colors);
}
#pragma endregion

// **************************************************
// *** Game Control
// **************************************************
#pragma region Game Control
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
	gamePaused = true;
#endif
	DEBUG_PRINT("Goal scored for team ");
	DEBUG_PRINT(wasGoalForTeam + 1);
	DEBUG_PRINT(", standing: ");
	DEBUG_PRINT(goals[0]);
	DEBUG_PRINT(":");
	DEBUG_PRINT(goals[1]);
	DEBUG_PRINTLN(".");

	wasGoal = false;
	wasGoalForTeam = -1;

	// Require update of OLED display
	updateOledRequired = true;
}

void updateGameTime(int decBy)
{
	if (gameTimeRemain > 0)
	{
		gameTimeRemain += decBy;
	}
	if (gameTimeRemain <= 0) // Game ended
	{
		gameRunning = false;
		gamePaused = true;
		//gameTimeRemain = gameTimeMax;
	}

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
#pragma endregion

// **************************************************
// *** OLED Display
// **************************************************
#pragma region OLED Display
// [Update OLED Display]
void drawDisplay()
{
	// clear the display
	display.clear();

	// -- HEADERS ("Heim" / "Gast")
	display.setColor(OLEDDISPLAY_COLOR::WHITE);
	display.setFont(Dialog_plain_12);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	//display.drawString(64, 0, ":");
	display.drawString(32, 0, teamName1);
	display.drawString(96, 0, teamName2);

	// -- GOALS ("0 : 0")
	display.setFont(Nimbus_Sans_L_Regular_Condensed_32);
	char goals_str[3];
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(64, 12, ":");
	//sprintf(goals_str, "%02d", goals[0]); // with leading zero
	sprintf(goals_str, "%d\n", goals[0]); // without leading zero
	display.drawString(32, 12, goals_str);
	//sprintf(goals_str, "%02d", goals[1]); // with leading zero
	sprintf(goals_str, "%d\n", goals[1]); // without leading zero
	display.drawString(96, 12, goals_str);

	// -- PROGRESS / REMAINING TIME
	int progbar = (gameTimeProgress * 128) / 100;
	display.drawRect(0, 50, 128, 14);
	if (gameRunning)
	{
		if (gamePaused)
		{
			// Outline only when game is paused
			display.drawRect(0, 50, progbar, 14);
		}
		else
		{
			// Filled box when game is running
			display.fillRect(0, 50, progbar, 14);
		}
	}
	display.setFont(Dialog_plain_12);
	char time_str[6];
	int time_s = gameTimeRemain % 60;
	int time_m = (gameTimeRemain - time_s) / 60;
	sprintf(time_str, "%02d:%02d\n", time_m, time_s);
	if (!gameRunning)
	{
		// When game is not started, show time centered
		display.setTextAlignment(TEXT_ALIGN_CENTER);
		display.setColor(OLEDDISPLAY_COLOR::WHITE);
		display.drawString(64, 50, time_str);
	}
	else
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
			// Inverse color because progress bar is only filled when game is running
			display.setColor(OLEDDISPLAY_COLOR::INVERSE);
			//display.setColor(OLEDDISPLAY_COLOR::BLACK);
			display.drawString(progbar - 2, 50, time_str);
		}
	}

	// Alternative 1: separate headers by crossed lines
	/*
  display.setColor(OLEDDISPLAY_COLOR::WHITE);
  display.drawVerticalLine(64, 0, 50);
  display.drawHorizontalLine(0, 14, 128);
  */
	// Alternative 2: highlight headers by filled rectangle
	display.setColor(OLEDDISPLAY_COLOR::INVERSE);
	display.fillRect(0, 0, 128, 14);
	// Outline around whole display
	display.setColor(OLEDDISPLAY_COLOR::WHITE);
	display.drawRect(0, 0, 128, 64);

	// write the buffer to the display
	display.display();
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
	if (gameRunning)
	{
		goalScoredByTeam(0);
	}
	else
	{
		changeColorForTeam = 0;
	}
}
void onGoalButtonTeam2()
{
	if (gameRunning)
	{
		goalScoredByTeam(1);
	}
	else
	{
		changeColorForTeam = 1;
	}
}
void onGoalSensorTeam1()
{
	goalScoredByTeam(0);
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
#ifdef BUTTONS_MAIN_CFG_ON_I2C
	uint8_t val = expander.digitalRead(6);
#else
	int val = digitalRead(BUTTON_MAIN_PIN);
#endif
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

	if (pressedFor > resetGameAfter)
	{
		gameRunning = false;
		DEBUG_PRINTLN("resetting game.");
		resetGame();
	}
	else
	{
		if (!gameRunning)
		{
			// Start game if not started
			gameRunning = true;
		}
		else
		{
			// Pause/Unpause if was started
			gamePaused = !gamePaused;
		}
		updateOledRequired = true;
		DEBUG_PRINTLN(gameRunning ? "game started." : "games paused.");
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
// *** Application Setup
// **************************************************
#pragma region Application Setup
void setup()
{
	Serial.begin(115200);
	delay(1000);

	// Attach ticker for game time
	tickerGametime.attach(1, onTickerGametime);

#ifdef i2cOLED
	// Initialize OLED display
	display.init();
	display.flipScreenVertically();
	drawDisplay();
#endif

	// Wire buttons and events (I2C or directly connected)
#ifdef BUTTONS_ON_I2C
	DEBUG_PRINT("PCF8574: setting bus speed to ");
	DEBUG_PRINT(I2C_BUS_SPEED);
	DEBUG_PRINTLN(".");
	Wire.setClock(I2C_BUS_SPEED);

	DEBUG_PRINTLN("PCF8574: setting PINs.");
	expander.pinMode(0, INPUT_PULLUP); // Goal 1, button
	expander.pinMode(1, INPUT_PULLUP); // Goal 1, sensor
	expander.pinMode(2, INPUT_PULLUP); // Goal 1, sensor
	expander.pinMode(3, INPUT_PULLUP); // Goal 2, button
	expander.pinMode(4, INPUT_PULLUP); // Goal 2, sensor
	expander.pinMode(5, INPUT_PULLUP); // Goal 2, sensor
	expander.pinMode(6, INPUT_PULLUP); // Main button
	expander.pinMode(7, INPUT_PULLUP); // Settings button
	DEBUG_PRINT("PCF8574: use I2C address ");
	DEBUG_PRINT(I2C_EXPANDER_ADDRESS);
	DEBUG_PRINTLN(".");

	DEBUG_PRINTLN("PCF8574: attaching ISR handler.");
	//expander.enableInterrupt(I2C_INT_PIN, ISRgateway);
	pinMode(I2C_INT_PIN, INPUT_PULLUP);
	attachInterrupt(I2C_INT_PIN, ISRgateway, FALLING);

	DEBUG_PRINTLN("PCF8574: attaching goal triggers.");
	/* Attach a software interrupt on pin 3 of the PCF8574 */
	expander.attachInterrupt(0, onGoalButtonTeam1, FALLING); // Manual button is pulling DOWN
	expander.attachInterrupt(3, onGoalButtonTeam2, FALLING); // Manual button is pulling DOWN
#ifdef SENSORS_ON_I2C
	expander.attachInterrupt(1, onGoalSensorTeam1, RISING); // IR _reflection_ trigger pulls UP
	expander.attachInterrupt(2, onGoalSensorTeam1, RISING); // IR _reflection_ trigger pulls UP
															//expander.attachInterrupt(2, onGoalSensorTeam1, FALLING); // IR _interruption_ trigger pulls DOWN
	expander.attachInterrupt(4, onGoalSensorTeam2, RISING); // IR _reflection_ trigger pulls UP
	expander.attachInterrupt(5, onGoalSensorTeam2, RISING); // IR _reflection_ trigger pulls UP
															//expander.attachInterrupt(5, onGoalSensorTeam2, FALLING); // IR _interruption_ trigger pulls DOWN
#endif

#ifdef BUTTONS_MAIN_CFG_ON_I2C
	DEBUG_PRINTLN("PCF8574: attaching main/cfg buttons.");
	expander.attachInterrupt(6, onMainButton, CHANGE);	 // Main button; must be able to handle long press to reset
	expander.attachInterrupt(7, onSettingsButton, RISING); // Settings button; safer to handle when released
#endif

	DEBUG_PRINTLN("PCF8574: start listening.");
	expander.begin(I2C_EXPANDER_ADDRESS);
#else
	DEBUG_PRINTLN("Attaching direct handlers for goal buttons.");
	pinMode(BUTTON_TEAM_1_PIN, INPUT_PULLUP);
	attachInterrupt(BUTTON_TEAM_1_PIN, onGoalButtonTeam1, FALLING);
	pinMode(BUTTON_TEAM_2_PIN, INPUT_PULLUP);
	attachInterrupt(BUTTON_TEAM_2_PIN, onGoalButtonTeam2, FALLING);
#endif

#ifndef BUTTONS_MAIN_CFG_ON_I2C
	DEBUG_PRINTLN("Attaching direct handlers for main/cfg buttons.");
	pinMode(BUTTON_MAIN_PIN, INPUT_PULLUP);
	attachInterrupt(BUTTON_MAIN_PIN, onMainButton, CHANGE);
	pinMode(BUTTON_SETTINGS_PIN, INPUT);
	attachInterrupt(BUTTON_SETTINGS_PIN, onSettingsButton, RISING);
#endif

	DEBUG_PRINTLN("FastLED: Initializing color palettes");
	InitColorNames();
	maxColNr = CommonColorNames.size();

	DEBUG_PRINTLN("FastLED: Initializing LED strip");
	initStrip(true, PLAY_DEMO);
	DEBUG_PRINT("FastLED: Amount of LEDs: ");
	DEBUG_PRINTLN(PIXEL_COUNT);

	DEBUG_PRINTLN("FastLED: Starting LED strip");
	startStrip();

	for (int grpNr = 0; grpNr < neoGroups.size(); grpNr++)
	{
		DEBUG_PRINT("FastLED: Setting up group #");
		DEBUG_PRINTLN(grpNr);
		//bool startFx = true;
		bool startFx = grpNr != 0;
#ifdef DO_NOT_START_FX_ON_INIT
		startFx = false;
#endif
		SetColors(grpNr, grpNr == 0 ? defaultColPalAll : defaultColPal);
		SetEffect(grpNr, grpNr == 0 ? defaultFxNrAll : defaultFxNr, startFx, grpNr == 0);
	}

#ifdef i2cOLED
	drawDisplay();
#endif
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

	if (ledsStarted)
	{
		if (changeColorForTeam >= 0)
		{
			int teamNr = changeColorForTeam;
			changeColorForTeam = -1;
			DEBUG_PRINT("Changing color of team #");
			DEBUG_PRINT(teamNr + 1);
			DEBUG_PRINT(" to ");
			TeamHueValues[teamNr] += 32;
			//DEBUG_PRINT(TeamHueValues[teamNr]);
			PrintHex8(TeamHueValues[teamNr]);
			DEBUG_PRINTLN(".");
			CreateTeamColorPalettes(teamNr);
			SetColors(teamNr + 1, "Idle");
		}

		bool isActiveMainGrp = (&(neoGroups.at(0)))->Active;
#ifdef DEBUG_LOOP
/*
				DEBUG_PRINT("Loop: Main group active -> ");
				DEBUG_PRINTLN(isActiveMainGrp);
	*/
#endif
		bool ledsUpdated = false;
		for (int grpNr = 0; grpNr < neoGroups.size(); grpNr++)
		{
			NeoGroup *neoGroup = &(neoGroups.at(grpNr));

			if (isActiveMainGrp && grpNr != 0)
			{
#ifdef DEBUG_LOOP
				DEBUG_PRINT("Loop: Skipping group #");
				DEBUG_PRINTLN(i);
#endif
				continue; // Don't update other groups if main group (all LEDs) is active
			}

#ifdef DEBUG_LOOP
/*
			DEBUG_PRINT("Loop: Updating group ");
			DEBUG_PRINTLN(i);
*/
#endif
			ledsUpdated |= neoGroup->Update();
		}

		if (ledsUpdated)
		{
#ifdef DEBUG_LOOP
			DEBUG_PRINTLN("Loop: Refreshing LEDs.");
#endif
			FastLED.show();
		}
	}
	else
	{
		//DEBUG_PRINTLN("Loop: LEDs not started, leaving loop.");
	}

	// REQUIRED: allow processing of interrupts etc
	delay(0);
	//yield();
}
#pragma endregion
