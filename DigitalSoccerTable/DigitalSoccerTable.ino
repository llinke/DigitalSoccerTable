#include <Arduino.h>

// **************************************************
// *** Compiler Flags
// **************************************************
#pragma region Compiler Flags
// --- WiFi -----------------------------------------
//#define INCLUDE_WIFI
// --- DEBUG ----------------------------------------
#define ENABLE_SERIAL_DEBUG
// --- Demo -----------------------------------------
#define PLAY_DEMO true
// --- MP3 Player -----------------------------------
#define MP3_PLAYER
#ifdef MP3_PLAYER
#define MP3_SERIAL_RX D5
#define MP3_SERIAL_TX D6
#define MP3_BUSY_PIN D7
#endif
// --- Buttons --------------------------------------
//#define SENSORS_ON_I2C
#define BUTTON_MAIN_PIN 1
#define BUTTON_SETTINGS_PIN 2
#define BUTTON_GOAL1_PIN 3
#define BUTTON_GOAL2_PIN 0
#ifdef SENSORS_ON_I2C
#define SENSOR1_GOAL1_PIN 7
#define SENSOR2_GOAL1_PIN 6
#define SENSOR1_GOAL2_PIN 5
#define SENSOR2_GOAL2_PIN 4
#endif
#pragma endregion
// **************************************************

// **************************************************
// *** Includes
// **************************************************
#pragma region Includes
#include "SerialDebug.h"
//#include <ArduinoSTL.h>
#include <vector>
#include <map>
#include <string.h>

#include "I2CInclude.h"
#include <Ticker.h>
#include <time.h>

#include "FastLedInclude.h"
#include "ColorPalettes.h"
#include "NeoGroup.cpp"

#ifdef MP3_PLAYER
// #include "DFMp3Player.cpp"
#include <SoftwareSerial.h>
#include "DFMiniMp3.h"
#endif

#ifdef INCLUDE_WIFI
#include <ESP8266WiFi.h>	  //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>		  //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>	  //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
//#include <ArduinoOTA.h>
//#include <ESP8266mDNS.h>
#endif
#pragma endregion
// **************************************************

// **************************************************
// *** Emnumerations
// **************************************************
#pragma region Enumerations
enum gamePhase
{
	GAME_BOOTED,
	GAME_IDLE,
	GAME_COINTOSS,
	GAME_KICKOFF,
	GAME_RUNNING,
	GAME_PAUSED,
	GAME_GOAL,
	GAME_CELEBRATION,
	GAME_OVER
};
#pragma endregion
// **************************************************

// **************************************************
// *** Constants Definitions
// **************************************************
#pragma region Constants Definitions
const int buttonLockDuration = 200;		 // ignore buttons for X ms, prevent prelling
const int resetGameAfter = 1000 * 1;	 // MainButton pressed for X sec resets game
const int lockedGoalDuration = 1000 * 2; // ignore goal buttons / sensors for X sec
const int gameTimeAbsMin = 60 * 1;		 //4; // min X min
const int gameTimeAbsMax = 60 * 15;		 // max X min
const int gameTimeDefault = 60 * 6;		 // default X min

const uint8_t globalBrightness = 128;
const uint8_t teamHueChangeValue = 32;

// 0: Wave, 1: Dynamic Wave, 2: Noise, 3: Confetti, 4: Fade, 5: Comet, 6: Orbit, 7: Fill
const uint8_t fxNrWave = 0;
const uint8_t fxNrDynamicWave = 1;
const uint8_t fxNrNoise = 2;
const uint8_t fxNrConfetti = 3;
const uint8_t fxNrFade = 4;
const uint8_t fxNrComet = 5;
const uint8_t fxNrOrbit = 6;
const uint8_t fxNrFill = 7;
const int maxFxNr = 5;
const int defaultFxNr = 1; //6;
const int defaultFxNrAll = 5;
const String defaultColPal = "Idle";
const String defaultColPalAll = "Rainbow";
const int defaultFps = 50;
const int defaultGlitter = 0;
const int defaultGlitterCelebration = 64;
const int fxGamePhaseIdle = fxNrDynamicWave;
const int fxGamePhaseCoinToss = fxNrFade;
const int fxGamePhaseKickOff = fxNrFade;
const int fxGamePhaseRunning = fxNrNoise;
const int fxGamePhasePaused = fxNrConfetti;
const int fxGamePhaseGoal = fxNrComet;
const int fxGamePhaseGoal2 = fxNrFill;
const int fxGamePhaseCelebration = fxNrFade;
const int fxGamePhaseOver = fxNrOrbit;

#ifdef MP3_PLAYER
// SFX to play on initialization
const uint8_t sfxFolderStart = 1;
volatile uint8_t sfxFilesStart = 0;
// SFX to play before games
const uint8_t sfxFolderIdle = 2;
volatile uint8_t sfxFilesIdle = 0;
// SFX to play during game
const uint8_t sfxFolderGame = 3;
volatile uint8_t sfxFilesGame = 0;
// SFX to play when celebrating
const uint8_t sfxFolderCelebration = 4;
volatile uint8_t sfxFilesCelebration = 0;
// SFX to play when goal was scored
const uint8_t sfxFolderGoal = 5;
volatile uint8_t sfxFilesGoal = 0;
// SFX to play when game is over
const uint8_t sfxFolderOver = 6;
volatile uint8_t sfxFilesOver = 0;

enum sfxMode
{
	SFX_START,
	SFX_IDLE,
	SFX_GAME,
	SFX_GOAL,
	SFX_CELEBRATION,
	SFX_OVER
};
volatile sfxMode sfxModeCurrent = SFX_IDLE;
volatile bool sfxNextRequested = false;
volatile int sfxNextRequestedAt = -1;
const int sfxNextRequestDelay = 2 * 1000;
const uint8_t sfxVolume = 8;

void RequestNextSfx(int requestedAt = 0)
{
	DEBUG_PRINTLN("MP3: REQUEST to play next track.");
	sfxNextRequested = true;

	if (requestedAt == 0 && !CanPlayNextSfx())
		return;

	sfxNextRequestedAt = requestedAt != 0 ? requestedAt : millis();
}

void ResetNextSfxRequest()
{
	sfxNextRequested = false;
	sfxNextRequestedAt = -1;
	DEBUG_PRINTLN("MP3: CLEAR request to play next track.");
}

bool CanPlayNextSfx()
{
	// DEBUG_PRINTLN("MP3: CanPlayNextSfx >> " + String(millis()) + ">" + String(sfxNextRequestedAt + sfxNextRequestDelay) + " ???");
	if ((sfxNextRequestedAt < 0) || (millis() > (sfxNextRequestedAt + sfxNextRequestDelay)))
	{
		DEBUG_PRINTLN("MP3: CanPlayNextSfx >> OK");
		return true;
	}

	// DEBUG_PRINTLN("MP3: CanPlayNextSfx >> IGNORED!");
	return false;
}

class DfPlayerNotify
{
  public:
	static void OnError(uint16_t errorCode)
	{
		DEBUG_PRINTLN("MP3: COM ERROR " + String(errorCode));
	}
	static void OnPlayFinished(uint16_t track)
	{
		DEBUG_PRINTLN("MP3: Play finished for #" + String(track));
		// RequestNextSfx();
	}
	static void OnCardOnline(uint16_t code)
	{
		DEBUG_PRINTLN("MP3: Card online.");
	}
	static void OnCardInserted(uint16_t code)
	{
		DEBUG_PRINTLN("MP3: Card inserted.");
	}
	static void OnCardRemoved(uint16_t code)
	{
		DEBUG_PRINTLN("MP3: Card removed.");
	}
};

SoftwareSerial dfSoftSerial(MP3_SERIAL_RX, MP3_SERIAL_TX); // RX, TX
DFMiniMp3<SoftwareSerial, DfPlayerNotify> dfPlayer(dfSoftSerial);
#endif
#pragma endregion
// **************************************************

// **************************************************
// *** Variable Declarations
// **************************************************
#pragma region Variable Declarations
// [Common Variables]
volatile bool buttonPressedOnI2C = false;
volatile bool updateOledRequired = false;
volatile bool buttonsLocked = false;
volatile int buttonsLockedAt = 0;

// [Game Control]
String teamName1 = "HEIM"; //"Dortmund";
String teamName2 = "GAST"; //"Bayern";
volatile bool gameRunning = false;
volatile bool gamePaused = false;
volatile bool mainButtonReleased = false;
volatile uint32_t mainButtonAt = 0;
volatile bool settingsButtonPressed = false;
volatile uint32_t settingsButtonAt = 0;
volatile gamePhase currentGamePhase = gamePhase::GAME_BOOTED;
volatile int currentGamePhaseStep = 0;
volatile int currentGamePhaseTeamNr = -1;

// [Goals / Standings]
volatile bool wasGoal = false;
volatile int wasGoalByTeam = -1;
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
		/*
		for (int dot = 0; dot < PIXEL_COUNT; dot++)
		{
			leds[dot] = CHSV(random8(), 255, 255);
			FastLED.show();
			delay(10);
			DEBUG_PRINT(".");
		}
		DEBUG_PRINTLN("DONE");
		delay(500);
		*/
		CRGBPalette16 colorPalette = NeoGroup::GenerateRGBPalette(CommonColorPalettes.find("Rainbow")->second);
		for (int dot = 0; dot < 256; dot++)
		{
			fadeToBlackBy(leds, PIXEL_COUNT, 8);
			int variant = (PIXEL_COUNT / 16);
			//int pos = ease8InOutQuad(dot) + random(0 - variant, 0 + variant);
			int pos = quadwave8(dot) + random(0 - variant, 0 + variant);
			int pos1 = map8(pos, 0, PIXEL_COUNT / 2);
			int pos2 = map8(pos, 0, PIXEL_COUNT / 2) + PIXEL_COUNT / 2;
			//DEBUG_PRINT("Setting pixel #");
			//DEBUG_PRINTLN(pos);
			int bright = random(64, 255);

			/*
			CRGB color = CHSV(random8(), 255, 255);
			nblend(leds[pos], color, 128);
			*/
			uint8_t colpos = dot + random8(16) - 8;
			nblend(leds[pos1], ColorFromPalette(colorPalette, colpos, bright), 128);
			nblend(leds[pos2], ColorFromPalette(colorPalette, colpos, bright), 128);

			FastLED.show();
			delay(5);
			DEBUG_PRINT(".");
		}
		DEBUG_PRINTLN("DONE");

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
#ifdef PIXEL_USE_OFFSET
#ifdef PIXEL_OFFSET_ZERO
	addGroup("All LEDs' group", 0, PIXEL_COUNT, (PIXEL_COUNT / 4));
#else
	addGroup("All LEDs' group", 0, PIXEL_COUNT, 0);
#endif
#else
	addGroup("All LEDs' group", 0, PIXEL_COUNT, 0);
#endif
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

	DEBUG_PRINTLN("Adding group '" + String(grpId) + "' at " + String(ledFirst) + " with size " + String(ledCount));

	// V1: new NeoGroup
	//NeoGroup *newGroup = new NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	//neoGroups.push_back(newGroup);
	// V2: NeoGroup w/o new
	NeoGroup newGroup = NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	neoGroups.push_back(newGroup);

	DEBUG_PRINTLN("GroupCount: " + String(neoGroups.size()));

	return neoGroups.size();
}

bool isGroupActive(int grpNr)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	return neoGroup->Active;
}

bool isGroupFadingOut(int grpNr)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	return !(neoGroup->Active) && (neoGroup->IsFadingOut());
}

int startGroup(int grpNr, bool runOnlyOnce = false)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Start(runOnlyOnce);

	//updateOledRequired = true;

	return grpNr;
}

int stopGroup(int grpNr, bool stopNow = false)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Stop(stopNow);

	//updateOledRequired = true;

	return grpNr;
}

int setGrpEffect(
	int grpNr,
	pattern pattern,
	int amountglitter = -1,
	int fps = 0,
	direction direction = FORWARD,
	mirror mirror = MIRROR0,
	wave wave = LINEAR,
	int speed = 1)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Stop(true);

	//int fxGlitter = amountglitter <= 0 ? neoGroup->GetGlitter() : amountglitter;
	int fxGlitter = amountglitter <= 0 ? defaultGlitter : amountglitter;
	//int fxFps = fps <= 0 ? neoGroup->GetFps() : fps;
	int fxFps = fps > 0 ? fps : defaultFps;

	uint16_t result = neoGroup->ConfigureEffect(pattern, fxGlitter, fxFps, direction, mirror, wave, speed);
	//neoGroup->Start();

	//updateOledRequired = true;

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

	//updateOledRequired = true;

	return result;
}

void SetEffect(int grpNr, int fxNr,
			   bool startFx, bool onlyOnce,
			   direction fxDirection = direction::FORWARD,
			   int amountGlitter = -1,
			   int targetFps = 0,
			   int speed = 1)
{
	DEBUG_PRINTLN("SetEffect ---------------------------------------------------");
	DEBUG_PRINTLN("Fx: Configuring LED effect #" + String(fxNr) + " for group #" + String(grpNr));

	String fxPatternName = "";
	pattern fxPattern = pattern::STATIC;
	int fxGlitter = amountGlitter >= 0 ? amountGlitter : defaultGlitter;
	int fxFps = defaultFps;
	mirror fxMirror = MIRROR0;
	wave fxWave = wave::LINEAR;

	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	switch (fxNr)
	{
	case fxNrWave:
		fxPatternName = "Wave";
		fxPattern = pattern::WAVE;
		fxMirror = mirror::MIRROR2;
		break;
	case fxNrDynamicWave:
		fxPatternName = "Dynamic Wave";
		fxPattern = pattern::DYNAMICWAVE;
		fxMirror = mirror::MIRROR1; // mirror::MIRROR0;
		break;
	case fxNrNoise:
		fxPatternName = "Noise";
		fxPattern = pattern::NOISE;
		fxMirror = mirror::MIRROR1; // mirror::MIRROR0;
		fxFps *= 2;					// double FPS looks better
		break;
	case fxNrConfetti:
		fxPatternName = "Confetti";
		fxPattern = pattern::CONFETTI;
		fxGlitter = 0;
		fxFps /= 2; // half FPS looks better
		break;
	case fxNrFade:
		fxPatternName = "Fade";
		fxPattern = pattern::FADE;
		fxFps /= 2; // half FPS looks better
		break;
	case fxNrComet:
		fxPatternName = "Comet";
		fxPattern = pattern::COMET;
		fxWave = wave::EASEINOUT;
		// fxWave = wave::SINUS;
		fxFps *= 3; // faster FPS looks better
		// fxMirror = mirror::MIRROR0;
		fxMirror = mirror::MIRROR2;
		break;
	case fxNrOrbit:
		fxPatternName = "Orbit";
		fxPattern = pattern::COMET;
		// fxWave = wave::EASEINOUT;
		fxWave = wave::SINUS;
		fxFps *= 1.5; // faster FPS looks better
		// fxMirror = mirror::MIRROR0;
		fxMirror = mirror::MIRROR2;
		break;
	case fxNrFill:
		fxPatternName = "Fill";
		fxPattern = pattern::FILL;
		fxWave = wave::EASEINOUT;
		fxFps *= 1.5; // faster FPS looks better
		// fxMirror = mirror::MIRROR0;
		fxMirror = mirror::MIRROR2;
		break;
	default:
		fxPatternName = "Static";
		fxPattern = pattern::STATIC;
		fxMirror = mirror::MIRROR0;
		break;
	}
	if (targetFps > 0)
		fxFps = targetFps;
	DEBUG_PRINTLN("Fx: Changing effect to '" + String(fxPatternName) + "'");
	setGrpEffect(
		grpNr,
		fxPattern,
		fxGlitter,
		fxFps,
		fxDirection,
		fxMirror,
		fxWave,
		speed);
	if (startFx)
		startGroup(grpNr, onlyOnce);
	DEBUG_PRINTLN("SetEffect ---------------------------------------------------");
}

void InitColorNames()
{
	InitColorPalettes();
}

void SetColors(int grpNr, String palKey, bool crossFade = CROSSFADE_PALETTES, int teamNr = -1, bool useTeamPalForGrp0 = true)
{
	DEBUG_PRINTLN("SetColors --------------------------------------------------");
	DEBUG_PRINTLN("Col: Configuring LED colors #" + String(palKey) + " for group #" + String(grpNr));

	DEBUG_PRINT("Col: Changing color palette to '" + String(palKey) + "' ");
	std::vector<CRGB> colors = {};
	if (grpNr == 0 && !useTeamPalForGrp0)
	{
		DEBUG_PRINTLN("of CommonColorPalettes.");
		if (CommonColorPalettes.find(palKey) != CommonColorPalettes.end())
			colors = CommonColorPalettes.find(palKey)->second;
	}
	else
	{
		DEBUG_PRINTLN("of TeamColorPalettes.");
		int teamPalNr = teamNr >= 0 ? teamNr : grpNr - 1;
		if (TeamColorPalettes[teamPalNr].find(palKey) != TeamColorPalettes[teamPalNr].end())
			colors = TeamColorPalettes[teamPalNr].find(palKey)->second;
	}
	setGrpColors(grpNr, colors, true, true, crossFade);
	DEBUG_PRINTLN("Dumping palette's colors...");
	DumpPalette(colors);
	DEBUG_PRINTLN("SetColors --------------------------------------------------");
}
#pragma endregion

// **************************************************
// *** Game Phases
// **************************************************
#pragma region Game Phases
bool isReadyForGamePhase(int grpNr, bool stopNow = false)
{
	if (isGroupActive(grpNr))
	{
		stopGroup(grpNr, stopNow);
		return false;
	}
	else
	{
		if (isGroupFadingOut(grpNr))
			return false; // wait until groups are stopped and faded out
	}
	return true;
}

bool isRunningOnce(int grpNr)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	return neoGroup->IsRunningOnce();
}

void changeGamePhase(gamePhase newGamePhase, int teamNr = -1)
{
	// if (currentGamePhase == newGamePhase)
	// 	return;
	DEBUG_PRINTLN("PHASE: changing to >> " + String(newGamePhase) + " for team " + String(currentGamePhaseTeamNr));
	currentGamePhaseStep = 0;
	currentGamePhaseTeamNr = teamNr;
	currentGamePhase = newGamePhase;
}

void playGamePhaseIdle()
{
	// -- <[ PREPARE ]> ----------
	if (currentGamePhaseStep == 0) // Prepare
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 1) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX.");
		SetColors(1, "Idle", true);
		SetColors(2, "Idle", true);
		SetEffect(1, fxGamePhaseIdle, true, false);
		SetEffect(2, fxGamePhaseIdle, true, false);
		currentGamePhaseStep++;
	}
	// Nothing more to do :-)
}

void playGamePhaseRunning()
{
	// -- <[ PREPARE ]> ----------
	if (currentGamePhaseStep == 0) // Prepare
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 1) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX.");
		SetColors(1, "InGame", true);
		SetColors(2, "InGame", true);
		SetEffect(1, fxGamePhaseRunning, true, false);
		SetEffect(2, fxGamePhaseRunning, true, false);
		currentGamePhaseStep++;
	}
	// Nothing more to do :-)
}

void playGamePhasePaused()
{
	// -- <[ PREPARE ]> ----------
	if (currentGamePhaseStep == 0) // Prepare
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 1) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX.");
		SetColors(1, "Idle", true);
		SetColors(2, "Idle", true);
		SetEffect(1, fxGamePhasePaused, true, false);
		SetEffect(2, fxGamePhasePaused, true, false);
		currentGamePhaseStep++;
	}
	// Nothing more to do :-)
}

void playGamePhaseCoinToss()
{
	// SetColors(1, "InGame", true);
	// SetColors(2, "InGame", true);
	// SetEffect(1, fxGamePhasePaused, true, false);
	// SetEffect(2, fxGamePhasePaused, true, false);
}

void playGamePhaseKickOff()
{
	// SetColors(1, "KickOff", false, teamNr);
	// SetColors(2, "KickOff", false, teamNr);
	// SetEffect(1, fxGamePhaseKickOff, true, false);
	// SetEffect(2, fxGamePhaseKickOff, true, false);
}

void playGamePhaseGoal()
{
	// -- <[ PREPARE ]> ----------
	if (currentGamePhaseStep == 0) // Prepare
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0, true);
		bool isReady1 = isReadyForGamePhase(1, true);
		bool isReady2 = isReadyForGamePhase(2, true);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 1) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX for " + String(currentGamePhaseTeamNr) + ".");
		/*
		SetColors(1, "Goal", false, currentGamePhaseTeamNr);
		SetColors(2, "Goal", false, currentGamePhaseTeamNr);
		SetEffect(1, fxGamePhaseGoal,
				  true, true,
				  currentGamePhaseTeamNr == 0 ? direction::FORWARD : direction::REVERSE,
				  defaultGlitter,
				  50,
				  2);
		SetEffect(2, fxGamePhaseGoal,
				  true, true,
				  currentGamePhaseTeamNr == 0 ? direction::REVERSE : direction::FORWARD,
				  defaultGlitter,
				  50,
				  2);
		*/
		SetColors(0, "Goal", false, currentGamePhaseTeamNr, true);
		SetEffect(0, fxGamePhaseGoal,
				  true, true,
				  currentGamePhaseTeamNr == 0 ? direction::FORWARD : direction::REVERSE,
				  defaultGlitter,
				  50,
				  2);
		currentGamePhaseStep++;
	}
	// -- <[ PLAY FX ]> ----------
	if (currentGamePhaseStep == 2) // Wait until FX is finished
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for runonce-FX to finish.");
		/*
		bool isRunningOnce1 = isRunningOnce(1);
		bool isRunningOnce2 = isRunningOnce(2);
		if (!isRunningOnce1 && !isRunningOnce2)
		{
			currentGamePhaseStep++;
		}
		*/
		bool isRunningOnce0 = isRunningOnce(0);
		if (!isRunningOnce0)
		{
			currentGamePhaseStep++;
		}
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 3) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX for " + String(currentGamePhaseTeamNr) + ".");
		/*
		SetColors(1, "Goal2", false, currentGamePhaseTeamNr);
		SetColors(2, "Goal2", false, currentGamePhaseTeamNr);
		SetEffect(1, fxGamePhaseGoal2,
				  true, true,
				  currentGamePhaseTeamNr == 0 ? direction::REVERSE : direction::FORWARD,
				  defaultGlitter,
				  75,
				  2);
		SetEffect(2, fxGamePhaseGoal2,
				  true, true,
				  currentGamePhaseTeamNr == 0 ? direction::FORWARD : direction::REVERSE,
				  defaultGlitter,
				  75,
				  2);
		*/
		SetColors(0, "Goal2", false, currentGamePhaseTeamNr, true);
		SetEffect(0, fxGamePhaseGoal2,
				  true, true,
				  currentGamePhaseTeamNr == 0 ? direction::REVERSE : direction::FORWARD,
				  defaultGlitter,
				  75,
				  2);
		currentGamePhaseStep++;
	}
	// -- <[ PLAY FX ]> ----------
	if (currentGamePhaseStep == 4) // Wait until FX is finished
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for runonce-FX to finish.");
		/*
		bool isRunningOnce1 = isRunningOnce(1);
		bool isRunningOnce2 = isRunningOnce(2);
		if (!isRunningOnce1 && !isRunningOnce2)
		{
			currentGamePhaseStep++;
		}
		*/
		bool isRunningOnce0 = isRunningOnce(0);
		if (!isRunningOnce0)
		{
			currentGamePhaseStep++;
		}
	}
	// -- <[ SWITCH GAME PHASE ]> ----------
	if (currentGamePhaseStep == 5) // Switch to next game phase
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
		{
			currentGamePhaseStep++;
			// Switch to game phase celebration
			changeGamePhase(gamePhase::GAME_CELEBRATION, currentGamePhaseTeamNr);

			lockedGoalTriggers = false;
		}
	}
	// Nothing more to do :-)
}

void playGamePhaseCelebration()
{
	// -- <[ PREPARE ]> ----------
	if (currentGamePhaseStep == 0) // Prepare
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 1) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX for " + String(currentGamePhaseTeamNr) + ".");
		/*
		SetColors(1, "Celebration", true, currentGamePhaseTeamNr);
		SetColors(2, "Celebration", true, currentGamePhaseTeamNr);
		SetEffect(1, fxGamePhaseCelebration, true, false, direction::FORWARD, defaultGlitterCelebration);
		SetEffect(2, fxGamePhaseCelebration, true, false, direction::FORWARD, defaultGlitterCelebration);
		*/
		SetColors(0, "Celebration", true, currentGamePhaseTeamNr, true);
		SetEffect(0, fxGamePhaseCelebration, true, false, direction::FORWARD, defaultGlitterCelebration);
		currentGamePhaseStep++;
	}
	// Nothing more to do :-)
}

void playGamePhaseGameOver()
{
	// -- <[ PREPARE ]> ----------
	if (currentGamePhaseStep == 0) // Prepare
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
			currentGamePhaseStep++;
	}
	// -- <[ START FX ]> ----------
	if (currentGamePhaseStep == 1) // Start FX
	{
		DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
					  "Starting FX.");
		SetColors(1, "Goal", true);
		SetColors(2, "Goal", true);
		SetEffect(1, fxGamePhaseOver,
				  true, true,
				  direction::FORWARD);
		SetEffect(2, fxGamePhaseOver,
				  true, true,
				  direction::FORWARD);
		currentGamePhaseStep++;
	}
	// -- <[ PLAY FX ]> ----------
	if (currentGamePhaseStep == 2) // Wait until FX is finished
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for runonce-FX to finish.");
		bool isRunningOnce1 = isRunningOnce(1);
		bool isRunningOnce2 = isRunningOnce(2);
		if (!isRunningOnce1 && !isRunningOnce2)
		{
			currentGamePhaseStep++;
		}
	}
	// -- <[ SWITCH GAME PHASE ]> ----------
	if (currentGamePhaseStep == 3) // Switch to next game phase
	{
		// DEBUG_PRINTLN("PHASE [" + String(currentGamePhase) + "-" + String(currentGamePhaseStep) + "]: " +
		// 			  "waiting for FX to stop.");
		bool isReady0 = isReadyForGamePhase(0);
		bool isReady1 = isReadyForGamePhase(1);
		bool isReady2 = isReadyForGamePhase(2);
		if (isReady0 && isReady1 && isReady2)
		{
			currentGamePhaseStep++;

			// Switch to game phase celebration
			if (goals[0] == goals[1]) // draw game
			{
				changeGamePhase(gamePhase::GAME_IDLE);
			}
			else
			{
				changeGamePhase(gamePhase::GAME_CELEBRATION, goals[0] > goals[1] ? 0 : 1);
			}
			currentGamePhaseStep++;
		}
	}
	// Nothing more to do :-)
}

void playGamePhase()
{
	switch (currentGamePhase)
	{
	case gamePhase::GAME_IDLE:
		playGamePhaseIdle();
		break;
	case gamePhase::GAME_RUNNING:
		playGamePhaseRunning();
		break;
	case gamePhase::GAME_PAUSED:
		playGamePhasePaused();
		break;
	case gamePhase::GAME_COINTOSS:
		playGamePhaseCoinToss();
		break;
	case gamePhase::GAME_KICKOFF:
		playGamePhaseKickOff();
		break;
	case gamePhase::GAME_GOAL:
		playGamePhaseGoal();
		break;
	case gamePhase::GAME_CELEBRATION:
		playGamePhaseCelebration();
		break;
	case gamePhase::GAME_OVER:
		playGamePhaseGameOver();
		break;
	default:
		break;
	}
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

	goals[wasGoalByTeam]++;
	lockedGoalTriggers = true;
	lockedGoalTriggersAt = millis();
	gamePaused = true;
	DEBUG_PRINTLN("Goal scored for team " + String(wasGoalByTeam + 1) + ", standing: " + String(goals[0]) + ":" + String(goals[1]) + ".");

#ifdef MP3_PLAYER
	SwitchSfxMode(SFX_GOAL, true);
#endif
	changeGamePhase(gamePhase::GAME_GOAL, wasGoalByTeam);

	wasGoal = false;
	wasGoalByTeam = -1;

	// Require update of OLED display
	updateOledRequired = true;
}

void updateGameTime(int decBy)
{
	if (decBy != 0) // ignore locks if time was not changed
	{
		if (!gameRunning)
			return;
		if (gamePaused)
			return;
		if (lockedGoalTriggers)
			return;
	}

	gameTimeChanged = false;

	if (gameTimeRemain > 0)
	{
		gameTimeRemain += decBy;
	}
	if (gameTimeRemain <= 0) // Game ended
	{
		gameRunning = false;
		gamePaused = true;
		//gameTimeRemain = gameTimeMax;

#ifdef MP3_PLAYER
		SwitchSfxMode(SFX_OVER, true);
#endif
		changeGamePhase(gamePhase::GAME_OVER);
	}

	gameTimeProgress = (gameTimeRemain * 100) / gameTimeMax;
	// DEBUG_PRINTLN("Remaining time: " + String(gameTimeRemain) + ", in %: " + String(gameTimeProgress) + ", changed by " + String(decBy) + "sec.");

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
	wasGoalByTeam = -1;
	lockedGoalTriggers = false;
	lockedGoalTriggersAt = millis();

	// Reset time settings
	gameTimeRemain = gameTimeMax;
	updateGameTime(0);

#ifdef MP3_PLAYER
	SwitchSfxMode(SFX_IDLE, true);
#endif
	changeGamePhase(gamePhase::GAME_IDLE);

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
void ISRgateway()
{
	//os_intr_lock();
	buttonPressedOnI2C = true;
	//expander.checkForInterrupt();
	//DEBUG_PRINTLN("PCF8574: change of buttons' state detected in ISRgateway.");
	//os_intr_unlock();
}

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
	if (areButtonsLocked())
		return;

	lockButtons();

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

	wasGoalByTeam = team;
	wasGoal = true;
}

// [Main Button]
void onMainButton()
{
	uint8_t val = expander.digitalRead(BUTTON_MAIN_PIN);
	if (val == 0)
	{
		if (areButtonsLocked())
			return;

		lockButtons();

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
		/*
		if (pressedFor < buttonLockDuration)
		{
			DEBUG_PRINTLN("IGNORED!");
			return; // ignore if press detected again during lock duration
		}
		*/
		mainButtonReleased = true;
	}
}

void handleMainButton()
{
	if (lockedGoalTriggers || //ignore, triggers still locked
		wasGoal)			  // ignore, already triggered
	{
		mainButtonReleased = false;
		return;
	}

	uint32_t pressedFor = millis() - mainButtonAt;
	DEBUG_PRINT("Button MAIN released, pressed for " + String(pressedFor) + "ms: ");

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

#ifdef MP3_PLAYER
			SwitchSfxMode(SFX_GAME, true);
#endif
			changeGamePhase(gamePhase::GAME_RUNNING);
		}
		else
		{
			// Pause/Unpause if was started
			gamePaused = !gamePaused;
			if (gamePaused)
			{
#ifdef MP3_PLAYER
				SwitchSfxMode(SFX_IDLE, true);
#endif
				changeGamePhase(gamePhase::GAME_PAUSED);
			}
			else
			{
#ifdef MP3_PLAYER
				SwitchSfxMode(SFX_GAME, true);
#endif
				changeGamePhase(gamePhase::GAME_RUNNING);
			}
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
	DEBUG_PRINTLN("Button SETTINGS pressed " + String(pressedAgo) + "ms ago.");
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

		DEBUG_PRINTLN("Changed game time to " + String(gameTimeMax) + "sec.");

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
	if (lockedGoalTriggers)
		return;

	// DEBUG_PRINTLN("onTickerGametime: changed");
	gameTimeChanged = true;
}

// [Button locking]
void lockButtons()
{
	buttonsLocked = true;
	buttonsLockedAt = millis();
}

bool areButtonsLocked()
{
	if (buttonsLocked)
	{
		uint32_t lockedAgo = buttonsLockedAt > 0 ? millis() - buttonsLockedAt : buttonLockDuration;
		if (lockedAgo < buttonLockDuration)
		{
			DEBUG_PRINT("Buttons STILL LOCKED, locked just ");
			DEBUG_PRINT(lockedAgo);
			DEBUG_PRINTLN("ms ago...");
			return true;
		}
		buttonsLocked = false;
	}
	//DEBUG_PRINTLN("Buttons not locked.");
	return false;
}
#pragma endregion

// **************************************************
// *** MP3 Player
// **************************************************
#pragma region MP3 Player
#ifdef MP3_PLAYER
void PlayNextSfx(bool keepMode = false)
{
	if (!CanPlayNextSfx())
		return;

	if (!keepMode)
	{
		if (sfxModeCurrent == SFX_START)
		{
			DEBUG_PRINTLN("MP3: Auto switching SFX mode from " + String(sfxModeCurrent) + " to " + String(SFX_IDLE));
			sfxModeCurrent = SFX_IDLE;
		}
		if (sfxModeCurrent == SFX_GOAL)
		{
			DEBUG_PRINTLN("MP3: Auto switching SFX mode from " + String(sfxModeCurrent) + " to " + String(SFX_CELEBRATION));
			sfxModeCurrent = SFX_CELEBRATION;
		}
		else if (sfxModeCurrent == SFX_OVER)
		{
			DEBUG_PRINTLN("MP3: Auto switching SFX mode from " + String(sfxModeCurrent) + " to " + String(SFX_CELEBRATION));
			sfxModeCurrent = SFX_CELEBRATION;
		}
	}
	else
	{
		DEBUG_PRINTLN("MP3: Keeping current SFX mode " + String(sfxModeCurrent));
	}

	DEBUG_PRINTLN("MP3: Randomly selecting next MP3 to play for mode " + String(sfxModeCurrent) + "...");
	uint8_t nextFolder = 1;
	uint8_t nextTrack = 1;
	switch (sfxModeCurrent)
	{
	case SFX_START:
		nextFolder = sfxFolderStart;
		if (sfxFilesStart == 0)
		{
			sfxFilesStart = dfPlayer.getFolderTrackCount(nextFolder);
			DEBUG_PRINTLN("MP3: # of files in folder " + String(nextFolder) + ": " + String(sfxFilesStart));
		}
		nextTrack = random(0, sfxFilesStart);
		break;
	case SFX_IDLE:
		nextFolder = sfxFolderIdle;
		if (sfxFilesIdle == 0)
		{
			sfxFilesIdle = dfPlayer.getFolderTrackCount(nextFolder);
			DEBUG_PRINTLN("MP3: # of files in folder " + String(nextFolder) + ": " + String(sfxFilesIdle));
		}
		nextTrack = random(0, sfxFilesIdle);
		break;
	case SFX_GAME:
		nextFolder = sfxFolderGame;
		if (sfxFilesGame == 0)
		{
			sfxFilesGame = dfPlayer.getFolderTrackCount(nextFolder);
			DEBUG_PRINTLN("MP3: # of files in folder " + String(nextFolder) + ": " + String(sfxFilesGame));
		}
		nextTrack = random(0, sfxFilesGame);
		break;
	case SFX_GOAL:
		nextFolder = sfxFolderGoal;
		if (sfxFilesGoal == 0)
		{
			sfxFilesGoal = dfPlayer.getFolderTrackCount(nextFolder);
			DEBUG_PRINTLN("MP3: # of files in folder " + String(nextFolder) + ": " + String(sfxFilesGoal));
		}
		nextTrack = random(0, sfxFilesGoal);
		break;
	case SFX_CELEBRATION:
		nextFolder = sfxFolderCelebration;
		if (sfxFilesCelebration == 0)
		{
			sfxFilesCelebration = dfPlayer.getFolderTrackCount(nextFolder);
			DEBUG_PRINTLN("MP3: # of files in folder " + String(nextFolder) + ": " + String(sfxFilesCelebration));
		}
		nextTrack = random(0, sfxFilesCelebration);
		break;
	case SFX_OVER:
		nextFolder = sfxFolderOver;
		if (sfxFilesOver == 0)
		{
			sfxFilesOver = dfPlayer.getFolderTrackCount(nextFolder);
			DEBUG_PRINTLN("MP3: # of files in folder " + String(nextFolder) + ": " + String(sfxFilesOver));
		}
		nextTrack = random(0, sfxFilesOver);
		break;
	}

	DEBUG_PRINTLN("MP3: Playing folder " + String(nextFolder) + ", track " + String(1 + nextTrack) + "...");
	dfPlayer.playFolderTrack(nextFolder, 1 + nextTrack);

	uint16_t currTrack = dfPlayer.getCurrentTrack();
	DEBUG_PRINTLN("MP3: Track playing " + String(currTrack));

	// DEBUG_PRINTLN("MP3: Getting status...");
	// uint16_t status = dfPlayer.getStatus();
	// DEBUG_PRINTLN("MP3: Status " + String(status));

	ResetNextSfxRequest();
}

void SwitchSfxMode(sfxMode sfxModeNext, bool switchNow)
{
	DEBUG_PRINTLN("MP3: Switching mode to " + String(sfxModeNext) + "...");
	sfxModeCurrent = sfxModeNext;
	if (switchNow)
		RequestNextSfx(-1);
}

void onDfPlayerBusyReleased()
{
	DEBUG_PRINTLN("MP3: BUSY signal released...");
	RequestNextSfx();
}

void SetupMp3Player()
{
	pinMode(MP3_BUSY_PIN, INPUT_PULLUP);
	attachInterrupt(MP3_BUSY_PIN, onDfPlayerBusyReleased, RISING);

	delay(1000);

	DEBUG_PRINTLN("MP3: Configuring SoftSerial...");
	dfSoftSerial.begin(9600);
	dfSoftSerial.setTimeout(600);

	DEBUG_PRINTLN("MP3: Initializing...");
	dfPlayer.begin();
	DEBUG_PRINTLN("MP3: Resetting...");
	dfPlayer.reset();

	delay(1000);

	DEBUG_PRINTLN("MP3: Getting status...");
	uint16_t status = dfPlayer.getStatus();
	DEBUG_PRINTLN("MP3: Status " + String(status));

	DEBUG_PRINTLN("MP3: Changing volume...");
	dfPlayer.setVolume(sfxVolume);
	uint16_t volume = dfPlayer.getVolume();
	DEBUG_PRINTLN("MP3: Volume " + String(volume));

	DEBUG_PRINTLN("MP3: Getting number of MP3s...");
	uint16_t count = dfPlayer.getTotalTrackCount();
	DEBUG_PRINTLN("MP3: Files " + String(count));

	DEBUG_PRINTLN("MP3: Setting repeat mode...");
	dfPlayer.setRepeatPlay(false);
	uint16_t mode = dfPlayer.getPlaybackMode();
	DEBUG_PRINTLN("MP3: Playback mode " + String(mode));

	DEBUG_PRINTLN("MP3: Setting equalizer...");
	dfPlayer.setEq(DfMp3_Eq_Normal);
	DfMp3_Eq eqmode = dfPlayer.getEq();
	DEBUG_PRINTLN("MP3: EQ " + String(eqmode));

	SwitchSfxMode(SFX_START, true);
	PlayNextSfx(true);

	DEBUG_PRINTLN("MP3: Setup finished!");
}

void RunMp3PlayerLoop()
{
	// calling dfPlayer.loop() periodically allows for notifications
	// to be handled without interrupts
	dfPlayer.loop();
}
#endif
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
	display.flipScreenVertically();
	drawDisplay();

	// Wire buttons and events (I2C or directly connected)
	DEBUG_PRINTLN("PCF8574: setting bus speed to " + String(I2C_BUS_SPEED) + ".");
	Wire.setClock(I2C_BUS_SPEED);

	DEBUG_PRINTLN("PCF8574: setting PINs.");
	expander.pinMode(BUTTON_MAIN_PIN, INPUT_PULLUP);	 // Main button
	expander.pinMode(BUTTON_SETTINGS_PIN, INPUT_PULLUP); // Settings button
	expander.pinMode(BUTTON_GOAL1_PIN, INPUT_PULLUP);	// Goal 1, button
	expander.pinMode(BUTTON_GOAL2_PIN, INPUT_PULLUP);	// Goal 2, button
#ifdef SENSORS_ON_I2C
	expander.pinMode(SENSOR1_GOAL1_PIN, INPUT_PULLUP); // Goal 1, sensor
	expander.pinMode(SENSOR2_GOAL1_PIN, INPUT_PULLUP); // Goal 1, sensor
	expander.pinMode(SENSOR1_GOAL2_PIN, INPUT_PULLUP); // Goal 2, sensor
	expander.pinMode(SENSOR2_GOAL2_PIN, INPUT_PULLUP); // Goal 2, sensor
#endif
	DEBUG_PRINTLN("PCF8574: use I2C address " + String(I2C_EXPANDER_ADDRESS) + ".");

	DEBUG_PRINTLN("PCF8574: attaching ISR handler.");
	//expander.enableInterrupt(I2C_INT_PIN, ISRgateway);
	pinMode(I2C_INT_PIN, INPUT_PULLUP);
	attachInterrupt(I2C_INT_PIN, ISRgateway, FALLING);

	DEBUG_PRINTLN("PCF8574: attaching goal triggers.");
	/* Attach a software interrupt on pin 3 of the PCF8574 */
	expander.attachInterrupt(BUTTON_GOAL1_PIN, onGoalButtonTeam1, FALLING); // Manual button is pulling DOWN
	expander.attachInterrupt(BUTTON_GOAL2_PIN, onGoalButtonTeam2, FALLING); // Manual button is pulling DOWN
#ifdef SENSORS_ON_I2C
	expander.attachInterrupt(SENSOR1_GOAL1_PIN, onGoalSensorTeam1, RISING); // IR _reflection_ trigger pulls UP
	expander.attachInterrupt(SENSOR2_GOAL1_PIN, onGoalSensorTeam1, RISING); // IR _reflection_ trigger pulls UP
																			//expander.attachInterrupt(2, onGoalSensorTeam1, FALLING); // IR _interruption_ trigger pulls DOWN
	expander.attachInterrupt(SENSOR1_GOAL2_PIN, onGoalSensorTeam2, RISING); // IR _reflection_ trigger pulls UP
	expander.attachInterrupt(SENSOR2_GOAL2_PIN, onGoalSensorTeam2, RISING); // IR _reflection_ trigger pulls UP
																			//expander.attachInterrupt(5, onGoalSensorTeam2, FALLING); // IR _interruption_ trigger pulls DOWN
#endif

#ifdef MP3_PLAYER
	SetupMp3Player();
	SwitchSfxMode(SFX_IDLE, false);
#endif

	DEBUG_PRINTLN("PCF8574: attaching main/cfg buttons.");
	expander.attachInterrupt(BUTTON_MAIN_PIN, onMainButton, CHANGE);		 // Main button; must be able to handle long press to reset
	expander.attachInterrupt(BUTTON_SETTINGS_PIN, onSettingsButton, RISING); // Settings button; safer to handle when released

	DEBUG_PRINTLN("PCF8574: start listening.");
	expander.begin(I2C_EXPANDER_ADDRESS);

	DEBUG_PRINTLN("FastLED: Initializing color palettes");
	InitColorNames();
	maxColNr = CommonColorNames.size();

	DEBUG_PRINTLN("FastLED: Initializing LED strip");
	initStrip(true, PLAY_DEMO);
	DEBUG_PRINTLN("FastLED: Amount of LEDs: " + String(PIXEL_COUNT));

	DEBUG_PRINTLN("FastLED: Starting LED strip");
	startStrip();

	for (int grpNr = 0; grpNr < neoGroups.size(); grpNr++)
	{
		DEBUG_PRINTLN("FastLED: Setting up group #" + String(grpNr));
		//bool startFx = true;
		//bool startFx = grpNr != 0;
		bool startFx = false;
		SetColors(grpNr, grpNr == 0 ? defaultColPalAll : defaultColPal, true, 0, false);
		SetEffect(grpNr, grpNr == 0 ? defaultFxNrAll : defaultFxNr, startFx, grpNr == 0);
	}
	changeGamePhase(gamePhase::GAME_IDLE);

	drawDisplay();
}
#pragma endregion

// **************************************************
// *** Application Loop
// **************************************************
#pragma region Application Loop
void loop()
{
	// Evaluate expander pins and execute attached callbacks
	if (buttonPressedOnI2C)
	{
		buttonPressedOnI2C = false;
		DEBUG_PRINTLN("PCF8574: change of buttons' state detected in ISRgateway.");
		expander.checkForInterrupt();
	}

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
	// if (lockedGoalTriggers &&
	// 	((millis() - lockedGoalTriggersAt) > lockedGoalDuration))
	// {
	// 	// Release lock after X ms
	// 	lockedGoalTriggers = false;
	// 	gamePaused = false;
	// 	updateOledRequired = true;
	// 	DEBUG_PRINTLN("Goal lock released.");
	// }

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
			DEBUG_PRINT("Changing color of team #" + String(teamNr + 1) + " to ");
			TeamHueValues[teamNr] += teamHueChangeValue;
			PrintHex8(TeamHueValues[teamNr]);
			DEBUG_PRINTLN(".");
			CreateTeamColorPalettes(teamNr);
			SetColors(teamNr + 1, "Idle");
		}

		playGamePhase();

		bool isActiveMainGrp =
			(&(neoGroups.at(0)))->Active ||
			(&(neoGroups.at(0)))->IsFadingOut();
		//DEBUG_PRINTLN("Loop: Main group active -> " + String(isActiveMainGrp));
		bool ledsUpdated = false;
		for (int grpNr = 0; grpNr < neoGroups.size(); grpNr++)
		{
			NeoGroup *neoGroup = &(neoGroups.at(grpNr));

			if (isActiveMainGrp && grpNr != 0)
			{
				//DEBUG_PRINTLN("Loop: Skipping group #" + String(i));
				continue; // Don't update other groups if main group (all LEDs) is active
			}

			//DEBUG_PRINTLN("Loop: Updating group " + String(i));
			ledsUpdated |= neoGroup->Update();
		}

		if (ledsUpdated)
		{
			//DEBUG_PRINTLN("Loop: Refreshing LEDs.");
			FastLED.show();
		}
	}
	else
	{
		////DEBUG_PRINTLN("Loop: LEDs not started, leaving loop.");
	}

#ifdef MP3_PLAYER
	RunMp3PlayerLoop();

	if (sfxNextRequested)
	{
		PlayNextSfx(sfxNextRequestedAt == -1);
	}
#endif

	// REQUIRED: allow processing of interrupts etc
	delay(0);
	//yield();
}
#pragma endregion
