#include "SerialDebug.h"
#include "FastLedInclude.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
//#include <map>

#define FIRE_COOLING 55
#define FIRE_SPARKING 120

#define FADEOUT_DURATION 500
#define FADEOUT_STEPS 16
#define CROSSFADE_PALETTES false
#define CROSSFADE_MAX_CHANGE_PER_STEP 16

#pragma region Enums
enum pattern
{
	NOFX,
	STATIC,
	FILL,
	FADE,
	WAVE,
	DYNAMICWAVE,
	NOISE,
	RAINBOW,
	CONFETTI,
	FIRE,
	COMET
};

enum wave
{
	LINEAR,
	SINUS,
	/*
	EASEIN,
	EASEOUT,
	*/
	EASEINOUT
};

enum direction
{
	FORWARD,
	REVERSE
};

enum mirror
{
	MIRROR0,
	MIRROR1,
	MIRROR2
};
#pragma endregion

class NeoGroup
{
#ifndef PIXEL_USE_OFFSET
	CRGB *LedFirst;
#endif
	int LedOffset = 0;

	uint8_t fxFps;
	unsigned long lastUpdate;
	int fxStep;
	direction fxDirection;
	wave fxWave;
	int fxFadeOut = 0;
	int fxAmountGlitter;
	uint8_t fxLength;
	mirror fxMirror = MIRROR0;
	bool onlyOnce = false;

	//std::vector<CRGB> currentColors = {};
	bool crossFadeColors = false;
	std::vector<CRGB> currentColors = {0x000000, 0x000000};
	CRGBPalette16 colorPalette = CRGBPalette16();
	CRGBPalette16 colorPaletteNew = CRGBPalette16();
	uint8_t firePaletteNr = 6;

	void (NeoGroup::*effectFunc)();

  public:
	String GroupID;
	int LedFirstNr;
	int LedCount;
	bool Active;

#pragma region Constructor
	NeoGroup(String groupID, int ledFirst, int ledCount, int ledOffset = 0)
	{
		GroupID = groupID;
		Active = false;
		fxStep = 0;
		LedFirstNr = ledFirst;
#ifndef PIXEL_USE_OFFSET
		LedFirst = &leds[ledFirst];
#endif
		LedCount = ledCount;
		LedOffset = ledOffset;
		colorPalette = GenerateRGBPalette({0x000000, 0x000000});
	}
#pragma endregion

#pragma region NeoGroup functions
	CRGB GetColorFromPaletteAt(uint8_t idx, uint8_t brightness = 255)
	{
		return ColorFromPalette(colorPalette, idx, brightness);
	}

	uint16_t ConfigureEffect(
		pattern pattern,
		int amountglitter = 0,
		uint8_t fps = 50,
		direction direction = FORWARD,
		mirror mirror = MIRROR0,
		wave wave = LINEAR)
	{
		DEBUG_PRINTLN("CfgFX: Stopping effect execution.");
		Stop();

		DEBUG_PRINTLN("CfgFX: Configuring effect parameters.");
		ChangeFps(fps);
		fxDirection = direction;
		fxStep = (fxDirection == REVERSE) ? 255 : 0;
		fxWave = wave;
		ChangeGlitter(amountglitter);
		fxMirror = mirror;
		fxLength = 255;

		if (pattern == STATIC)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Static' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxStatic;
		}
		if (pattern == FILL)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Fill' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxFill;
		}
		if (pattern == FADE)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Fade' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxFade;
		}
		if (pattern == WAVE)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Wave' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxWave;
			fxLength = constrain((LedCount * 1.5), 0, 255);
		}
		if (pattern == DYNAMICWAVE)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Dynamic Wave' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxColorWaves;
		}
		if (pattern == NOISE)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Noise' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxNoise;
		}
		if (pattern == RAINBOW)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Rainbow' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxRainbow;
			fxLength = constrain((LedCount * 2), 0, 255);
		}
		if (pattern == CONFETTI)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Confetti' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			effectFunc = &NeoGroup::FxConfetti;
			fxAmountGlitter = 0;
		}
		if (pattern == FIRE)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Fire' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
#ifdef PIXEL_USE_OFFSET
			FillPixels(0, LedCount, 0x000000);
#else
			fill_solid(LedFirst, LedCount, 0x000000);
#endif
			effectFunc = &NeoGroup::FxFire;
			fxAmountGlitter = 0;
		}
		if (pattern == COMET)
		{
			DEBUG_PRINT("CfgFX: Setting FX 'Comet' for group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
#ifdef PIXEL_USE_OFFSET
			FillPixels(0, LedCount, 0x000000);
#else
			fill_solid(LedFirst, LedCount, 0x000000);
#endif
			effectFunc = &NeoGroup::FxComet;
			fxAmountGlitter = 0;
		}
		return 0;
	}

	uint16_t ConfigureColors(
		std::vector<CRGB> colors,
		bool clearFirst = true,
		bool generatePalette = true,
		bool crossFade = false)
	{
		DEBUG_PRINT("CfgColor: Configuring colors for group '");
		DEBUG_PRINT(GroupID);
		DEBUG_PRINTLN("'.");
		if (clearFirst)
		{
			DEBUG_PRINTLN("CfgColor: Clearing old list of colors.");
			currentColors.clear();
		}

		DEBUG_PRINT("CfgColor: Adding ");
		DEBUG_PRINT(colors.size());
		DEBUG_PRINTLN(" CRGB colors to internal list.");
		for (CRGB color : colors)
		{
			currentColors.push_back(color);
		}

		if (generatePalette)
		{
			if (currentColors.size() != 0)
			{
				crossFadeColors = crossFade;
				if (crossFadeColors)
				{
					DEBUG_PRINTLN("CfgColor: Crossfading to new palette.");
					colorPaletteNew = GenerateRGBPalette(currentColors);
				}
				else
				{
					DEBUG_PRINTLN("CfgColor: Switching to new palette.");
					colorPalette = GenerateRGBPalette(currentColors);
				}
			}
			else
			{
				DEBUG_PRINTLN("CfgColor: No colors, using empty list.");
				if (crossFadeColors)
				{
					colorPaletteNew = GenerateRGBPalette({});
				}
				else
				{
					colorPalette = GenerateRGBPalette({});
				}
			}
		}
		return currentColors.size();
	}

	void ChangeFps(uint8_t fps)
	{
		fxFps = fps;
	}

	uint8_t GetFps()
	{
		return fxFps;
	}

	void ChangeGlitter(int amountglitter)
	{
		fxAmountGlitter = amountglitter;
	}

	int GetGlitter()
	{
		return fxAmountGlitter;
	}

	void Start(bool runOnlyOnce = false)
	{
		DEBUG_PRINT("GRP: Starting group '");
		DEBUG_PRINT(GroupID);
		DEBUG_PRINTLN("'.");
		fxStep = 0;
		onlyOnce = runOnlyOnce;
		if (onlyOnce)
			DEBUG_PRINTLN("GRP: Will run only ONCE.");
		Active = true;
		lastUpdate = 0;
	}

	void Stop(bool stopNow = false)
	{
		DEBUG_PRINT("GRP: Stopping group '");
		DEBUG_PRINT(GroupID);
		DEBUG_PRINTLN("'.");
		Active = false;
		fxFadeOut = (stopNow) ? 0 : FADEOUT_STEPS;
		if (stopNow)
		{
#ifdef PIXEL_USE_OFFSET
			FillPixels(0, LedCount, 0x000000);
#else
			fill_solid(LedFirst, LedCount, 0x000000);
#endif
		}
	}

	bool Update()
	{
		if (fxFadeOut > 0)
		{
			if ((millis() - lastUpdate) > (FADEOUT_DURATION / FADEOUT_STEPS))
			{
				/*
				DEBUG_PRINT("GRP: Fading out group '");
				DEBUG_PRINT(GroupID);
				DEBUG_PRINTLN("', ");
				DEBUG_PRINT(fxFadeOut);
				DEBUG_PRINTLN(" steps remaing.");
				*/
				lastUpdate = millis();
#ifdef PIXEL_USE_OFFSET
				fadeToBlackBy(&leds[LedFirstNr], LedCount, (1024 / FADEOUT_STEPS));
#else
				fadeToBlackBy(LedFirst, LedCount, (1024 / FADEOUT_STEPS));
#endif
				fxFadeOut--;
				if (fxFadeOut == 0)
				{
#ifdef PIXEL_USE_OFFSET
					FillPixels(0, LedCount, 0x000000);
#else
					fill_solid(LedFirst, LedCount, 0x000000);
#endif
					if (crossFadeColors)
					{
						colorPalette = GenerateRGBPalette({0x000000, 0x000000});
					}
				}
				return true; // LEDs updated
			}
			return false; // LEDs not updated
		}

		if (!Active)
		{
			/*
			DEBUG_PRINT("GRP: SKIPPED update for '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("', not active.");
			*/
			return false; // LEDs not updated
		}

		int updateInterval = (1000 / fxFps);
		if ((millis() - lastUpdate) > updateInterval)
		{
			/*
			DEBUG_PRINT("GRP: Updating group '");
			DEBUG_PRINT(GroupID);
			DEBUG_PRINTLN("'.");
			*/
			lastUpdate = millis();
			if (crossFadeColors)
			{
				// Cross-fade to new palette
				nblendPaletteTowardPalette(colorPalette, colorPaletteNew, CROSSFADE_MAX_CHANGE_PER_STEP);
			}
			if (effectFunc != NULL)
			{
				(this->*effectFunc)();

				if (fxAmountGlitter > 0)
				{
					FxGlitter(fxAmountGlitter);
				}
			}
			else
			{
				/*
				DEBUG_PRINT("GRP: ERROR, no FX function set for group '");
				DEBUG_PRINT(GroupID);
				DEBUG_PRINTLN("'.");
				*/
				return false;
			}
			return true; // LEDs updated
		}

		return false; // LEDs not updated
	}

	void NextFxStep(bool invert = false)
	{
		if (onlyOnce && ((!invert && fxStep == 255) || (invert && fxStep == 0)))
		{
			DEBUG_PRINTLN("NextFxStep: Run only once, stopping.");
			Stop();
			return;
		}

		if (fxDirection == FORWARD)
		{
			fxStep++;
			if (fxStep > 255)
			{
				if (invert)
				{
					ReverseFxDirection();
				}
				else
				{
					fxStep = 0;
				}
			}
		}
		else // fxDirection == REVERSE
		{
			fxStep--;
			if (fxStep < 0)
			{
				if (invert)
				{
					ReverseFxDirection();
				}
				else
				{
					fxStep = 255;
				}
			}
		}
		//DEBUG_PRINTLN("GRP: Group '" + String(GroupID) + "', next step: " + String(fxStep) + ".");
	}

	void ReverseFxDirection()
	{
		if (fxDirection == FORWARD)
		{
			fxDirection = REVERSE;
			fxStep = 255;
		}
		else
		{
			fxDirection = FORWARD;
			fxStep = 0;
		}
	}
#pragma endregion

#pragma region SetPixel functions
	void FillPixels(int pos, int count, CRGB newcolor, mirror mirror = MIRROR0, bool blend = false)
	{
		for (int p = pos; p < (pos + count); p++)
			SetPixel(p, newcolor, mirror, blend);
	}

	void SetPixel(int pos, CRGB newcolor, mirror mirror = MIRROR0, bool blend = false)
	{
		int ledNormalNr = -1;
		int ledMirrorNr = -1;

		if (mirror == MIRROR1) // set even/odd as mirror values
		{
			int newPos = pos / 2;
			ledNormalNr = ((pos % 2) != 0)
							  ? CapLedPosition(newPos + LedOffset, LedCount)
							  : CapLedPosition((LedCount - newPos - 1) + LedOffset, LedCount);
		}
		else if (mirror == MIRROR2) // mirror each second value
		{
			int newPos = pos / 2;
			int mirrorPos = LedCount - newPos - 1;
			ledNormalNr = CapLedPosition(newPos + LedOffset, LedCount);
			ledMirrorNr = CapLedPosition(mirrorPos + LedOffset, LedCount);
		}
		else
		{
			ledNormalNr = pos + LedOffset;
		}

#ifdef PIXEL_USE_OFFSET
		CRGB *LedTargetNormal =
			(ledNormalNr >= 0) ? &leds[CapLedPosition(LedFirstNr + ledNormalNr + PIXEL_OFFSET, PIXEL_COUNT)] : nullptr;
		CRGB *LedTargetMirror =
			(ledMirrorNr >= 0) ? &leds[CapLedPosition(LedFirstNr + ledMirrorNr + PIXEL_OFFSET, PIXEL_COUNT)] : nullptr;
#else
		CRGB *LedTargetNormal =
			(ledNormalNr >= 0) ? &LedFirst[ledNormalNr] : nullptr;
		CRGB *LedTargetMirror =
			(ledMirrorNr >= 0) ? &LedFirst[ledMirrorNr] : nullptr;
#endif

		if (blend)
		{
			if (ledNormalNr >= 0)
				nblend(LedTargetNormal[0], newcolor, 128);
			if (ledMirrorNr >= 0)
				nblend(LedTargetMirror[0], newcolor, 128);
		}
		else
		{
			if (ledNormalNr >= 0)
				LedTargetNormal[0] = newcolor;
			if (ledMirrorNr >= 0)
				LedTargetMirror[0] = newcolor;
		}
	}

	int CapLedPosition(int ledPos, int ledCount)
	{
		int newPos = ledPos;
		while (newPos >= ledCount)
			newPos -= ledCount;
		while (newPos < 0)
			newPos += ledCount;
		return newPos;
	}

	uint8_t GetEasedFxStep(uint8_t step)
	{
		int newStep = step;
		switch (fxWave)
		{
		case SINUS:
			newStep = quadwave8(step);
			break;
			/*
		case EASEIN:
			//newStep = ease8InOutQuad(step / 2 + step % 2) * 2 + step % 2;
			newStep = (ease8InOutQuad(step >> 1) << 1) | (step & 0x01);
			break;
		case EASEOUT:
			//newStep = (ease8InOutQuad(128 + (step / 2 + step % 2)) - 128) * 2 + step % 2;
			newStep = (constrain(ease8InOutQuad(0x80 | (step >> 1)), 0, 0x7f)) | (step & 0x01);
			break;
		case EASEINOUT:
*/
		case EASEINOUT:
			newStep = ease8InOutQuad(step);
			break;
		default:
			newStep = fxStep;
			break;
		}
		//if (GroupID == "Team 2 group")
		//	DEBUG_PRINTLN(String(GroupID) + ": GetEasedFxStep >> " + String(step) + " to " + String(newStep));
		return newStep;
	}
#pragma endregion

#pragma region Effects
	void FxStatic()
	{
		CRGB newColor = ColorFromPalette(colorPalette, 0);
#ifdef PIXEL_USE_OFFSET
		FillPixels(0, LedCount, newColor);
#else
		fill_solid(LedFirst, LedCount, newColor);
#endif
		for (int i = 0; i < LedCount; i++)
		{
			SetPixel((LedCount - 1) - i, newColor);
		}
		//Stop();
	}

	void FxFade()
	{
		CRGB newColor = ColorFromPalette(colorPalette, fxStep);
		//fill_solid(LedFirst, LedCount, newColor);
		FillPixels(0, LedCount, newColor);

		NextFxStep();
	}

	void FxFill()
	{
#ifdef PIXEL_USE_OFFSET
		fadeToBlackBy(&leds[LedFirstNr], LedCount, 16);
#else
		fadeToBlackBy(LedFirst, LedCount, 16);
#endif
		CRGB newColor = (fxDirection == REVERSE)
							? ColorFromPalette(colorPalette, 255 - fxStep)
							: ColorFromPalette(colorPalette, fxStep);
		//int variant = LedCount < 64 ? (LedCount / 4) : (LedCount / 8);
		if (fxDirection == REVERSE)
		{
			uint8_t pos = GetEasedFxStep(255 - fxStep); // + random(0 - variant, 0 + variant);
			int ledAmount = scale8(LedCount, pos);
			int ledPos = LedCount - ledAmount;
			if (ledAmount > 0)
				FillPixels(ledPos, ledAmount, newColor, fxMirror, true);
		}
		else
		{
			uint8_t pos = GetEasedFxStep(fxStep); // + random(0 - variant, 0 + variant);
			int ledAmount = scale8(LedCount, pos);
			int ledPos = 0;
			if (ledAmount > 0)
				FillPixels(ledPos, ledAmount, newColor, fxMirror, true);
		}

		NextFxStep();
	}

	void FxWave()
	{
		uint8_t waveLength = fxLength;
		uint8_t deltaWave = waveLength >= LedCount ? waveLength / LedCount : 1;
		//fill_rainbow(LedFirst, LedCount, fxStep, deltaHue);

		CRGB rgb;
		uint8_t wavePos = fxStep;
		for (int i = 0; i < LedCount; i++)
		{
			rgb = ColorFromPalette(colorPalette, wavePos);
			SetPixel((LedCount - i - 1), rgb, fxMirror);
			wavePos += deltaWave;
		}

		NextFxStep();
	}

	void FxRainbow()
	{
		uint8_t rainBowLength = fxLength;
		uint8_t deltaHue = rainBowLength >= LedCount ? rainBowLength / LedCount : 1;
		//fill_rainbow(LedFirst, LedCount, fxStep, deltaHue);

		CHSV hsv;
		hsv.hue = fxStep;
		hsv.sat = 240;
		hsv.val = 255;
		for (int i = 0; i < LedCount; i++)
		{
			//LedFirst[i] = hsv;
			SetPixel((LedCount - i - 1), hsv, fxMirror);
			hsv.hue += deltaHue;
		}

		NextFxStep();
	}

	void FxGlitter(uint8_t chanceOfGlitter = 80)
	{
		if (random8() < chanceOfGlitter)
		{
#ifdef PIXEL_USE_OFFSET
			SetPixel(random16(LedCount), CRGB::White, MIRROR0, true);
#else
			LedFirst[random16(LedCount)] += CRGB::White;
#endif
		}
	}

	void FxConfetti()
	{
#ifdef PIXEL_USE_OFFSET
		fadeToBlackBy(&leds[LedFirstNr], LedCount, 16);
#else
		fadeToBlackBy(LedFirst, LedCount, 16);
#endif
		int pos = random16(LedCount);
		SetPixel(pos, ColorFromPalette(colorPalette, fxStep + random8(64)), MIRROR0, false); //true);
		NextFxStep();
	}

	void FxComet()
	{
#ifdef PIXEL_USE_OFFSET
		fadeToBlackBy(&leds[LedFirstNr], LedCount, 16);
#else
		fadeToBlackBy(LedFirst, LedCount, 16);
#endif
		int pos = fxStep;
		int variant = LedCount < 64 ? (LedCount / 4) : (LedCount / 8);
		pos = GetEasedFxStep(fxStep) + random(0 - variant, 0 + variant);
		pos = lerp8by8(0, LedCount, pos); //(pos * LedCount) / 255;
		pos = constrain(pos, 0, LedCount);
		int bright = random(64, 255);
		uint8_t colpos = (fxStep * 1.5) + random8(16);
		SetPixel(pos, ColorFromPalette(colorPalette, colpos, bright), fxMirror, true);
		NextFxStep();
	}

	void FxFire()
	{
		// Array of temperature readings at each simulation cell
		byte heat[LedCount];

		// Step 1.  Cool down every cell a little
		for (int i = 0; i < LedCount; i++)
		{
			heat[i] = qsub8(heat[i], random8(0, ((FIRE_COOLING * 10) / LedCount) + 2));
		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		for (int k = LedCount - 1; k >= 2; k--)
		{
			heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
		}

		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		if (random8() < FIRE_SPARKING)
		{
			int y = random8(LedCount / 8);
			heat[y] = qadd8(heat[y], random8(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
		for (int j = 0; j < LedCount; j++)
		{
			// Scale the heat value from 0-255 down to 0-240
			// for best results with color palettes.
			byte colorindex = scale8(heat[j], 240);
			CRGB color = ColorFromPalette(colorPalette, colorindex);
			SetPixel(j, color, fxMirror);
		}
		NextFxStep();
	}

	void FxColorWaves()
	{
		static uint16_t sPseudotime = 0;
		static uint16_t sLastMillis = 0;
		static uint16_t sHue16 = 0;

		uint8_t sat8 = beatsin88(87, 220, 250);
		uint8_t brightdepth = beatsin88(341, 96, 224);
		uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
		uint8_t msmultiplier = beatsin88(147, 23, 60);

		uint16_t hue16 = sHue16; //gHue * 256;
		uint16_t hueinc16 = beatsin88(113, 300, 1500);

		uint16_t ms = millis();
		uint16_t deltams = ms - sLastMillis;
		sLastMillis = ms;
		sPseudotime += deltams * msmultiplier;
		sHue16 += deltams * beatsin88(400, 5, 9);
		uint16_t brightnesstheta16 = sPseudotime;

		for (uint16_t i = 0; i < LedCount; i++)
		{
			hue16 += hueinc16;
			uint8_t hue8 = hue16 / 256;
			uint16_t h16_128 = hue16 >> 7;
			if (h16_128 & 0x100)
			{
				hue8 = 255 - (h16_128 >> 1);
			}
			else
			{
				hue8 = h16_128 >> 1;
			}

			brightnesstheta16 += brightnessthetainc16;
			uint16_t b16 = sin16(brightnesstheta16) + 32768;

			uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
			uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
			bri8 += (255 - brightdepth);

			uint8_t index = hue8;
			//index = triwave8( index);
			index = scale8(index, 240);

			CRGB newcolor = ColorFromPalette(colorPalette, index, bri8);
			uint16_t pixelnumber = i;
			pixelnumber = (LedCount - 1) - pixelnumber;
			//nblend(ledarray[pixelnumber], newcolor, 128);
			SetPixel(pixelnumber, newcolor, fxMirror, true);
		}
	}

	void FxNoise()
	{
		static uint8_t lastMillis = 0;
		static uint8_t slowness = 2;
		static uint32_t noiseYScale = 0;

		uint8_t currentMillis = millis();
		if (uint8_t(currentMillis - lastMillis) > 8)
		{
			lastMillis = currentMillis;
			//noiseYScale++;
			noiseYScale += uint16_t(63) << slowness;
		}

		for (uint16_t i = 0; i < LedCount; i++)
		{
			// X location is constant, but we move along the Y at the rate of millis()
			//uint8_t index = inoise8(uint16_t(i) * 20, noiseYScale);
			//leds[i] = ColorFromPalette(palette, index);
			int16_t index = inoise16(uint32_t(i) << 12, noiseYScale);
			/*
			uint8_t hi4 = (index >> 12) & 0xFF; // take the highest 4 bits
			uint8_t lo8 = (index >> 4) & 0xFF;  // take the next-to-highest 8 bits, ignore the lowest 4 bits
			uint8_t index8 = (hi4 << 4) | (lo8 & 0x0F);
			*/
			//leds[i] = ColorFromPalette16(palette, index);
			uint16_t pixelnumber = i;
			pixelnumber = (LedCount - 1) - pixelnumber;
			/*
			DEBUG_PRINT("FX: Getting color from palette for pixel ");
			DEBUG_PRINT(pixelnumber);
			DEBUG_PRINT(" at index16 ");
			DEBUG_PRINT(index, HEX);
			DEBUG_PRINT(" as index8 ");
			DEBUG_PRINTLN(index8, HEX);
			*/
			CRGB newcolor = ColorFromPalette16(colorPalette, index);
			SetPixel(pixelnumber, newcolor, fxMirror);
		}
	}
#pragma endregion

#pragma region Static ColorPalette functions
	static CRGBPalette16 GenerateRGBPalette(std::vector<CRGB> colors)
	{
		CRGB nc[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		int colCount = colors.size();
		for (int c = 0; c < colCount; c++)
		{
			uint16_t trgtIdx1 = ((c * 16) / colCount);
			uint16_t trgtIdx2 = (((c + 1) * 16) / colCount) - 1;
			if (trgtIdx2 > 15)
				trgtIdx2 = 15;
			if (trgtIdx2 < 0)
				trgtIdx2 = 0;
			CRGB col1 = colors[c];
			CRGB col2 = (c + 1 < colCount) ? colors[c + 1] : colors[0];
			fill_gradient_RGB(&(nc[0]), trgtIdx1, col1, trgtIdx2, col2);
		}
		return CRGBPalette16(
			nc[0], nc[1], nc[2], nc[3], nc[4], nc[5], nc[6], nc[7],
			nc[8], nc[9], nc[10], nc[11], nc[12], nc[13], nc[14], nc[15]);
	}

	// Copied from:
	// https://github.com/FastLED/FastLED/blob/master/colorutils.cpp
	// Modified to have a 16-bit index, for more precise colors (though only
	// 12 bits are actually used).
	static CRGB ColorFromPalette16(const CRGBPalette16 &pal, uint16_t index, uint8_t brightness = 0xff, TBlendType blendType = LINEARBLEND)
	{
		uint8_t hi4 = index >> 12; // take the highest 4 bits
		uint8_t lo8 = index >> 4;  // take the next-to-highest 8 bits, ignore the lowest 4 bits

		// const CRGB* entry = &(pal[0]) + hi4;
		// since hi4 is always 0..15, hi4 * sizeof(CRGB) can be a single-byte value,
		// instead of the two byte 'int' that avr-gcc defaults to.
		// So, we multiply hi4 X sizeof(CRGB), giving hi4XsizeofCRGB;
		uint8_t hi4XsizeofCRGB = hi4 * sizeof(CRGB);
		// We then add that to a base array pointer.
		const CRGB *entry = (CRGB *)((uint8_t *)(&(pal[0])) + hi4XsizeofCRGB);

		uint8_t blend = lo8 && (blendType != NOBLEND);

		uint8_t red1 = entry->red;
		uint8_t green1 = entry->green;
		uint8_t blue1 = entry->blue;

		if (blend)
		{

			if (hi4 == 15)
			{
				entry = &(pal[0]);
			}
			else
			{
				entry++;
			}

			uint8_t f2 = lo8;
			uint8_t f1 = 255 - f2;

			//    rgb1.nscale8(f1);
			uint8_t red2 = entry->red;
			red1 = scale8_LEAVING_R1_DIRTY(red1, f1);
			red2 = scale8_LEAVING_R1_DIRTY(red2, f2);
			red1 += red2;

			uint8_t green2 = entry->green;
			green1 = scale8_LEAVING_R1_DIRTY(green1, f1);
			green2 = scale8_LEAVING_R1_DIRTY(green2, f2);
			green1 += green2;

			uint8_t blue2 = entry->blue;
			blue1 = scale8_LEAVING_R1_DIRTY(blue1, f1);
			blue2 = scale8_LEAVING_R1_DIRTY(blue2, f2);
			blue1 += blue2;

			cleanup_R1();
		}

		if (brightness != 255)
		{
			if (brightness)
			{
				brightness++; // adjust for rounding
				// Now, since brightness is nonzero, we don't need the full scale8_video logic;
				// we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
				if (red1)
				{
					red1 = scale8_LEAVING_R1_DIRTY(red1, brightness);
#if !(FASTLED_SCALE8_FIXED == 1)
					red1++;
#endif
				}
				if (green1)
				{
					green1 = scale8_LEAVING_R1_DIRTY(green1, brightness);
#if !(FASTLED_SCALE8_FIXED == 1)
					green1++;
#endif
				}
				if (blue1)
				{
					blue1 = scale8_LEAVING_R1_DIRTY(blue1, brightness);
#if !(FASTLED_SCALE8_FIXED == 1)
					blue1++;
#endif
				}
				cleanup_R1();
			}
			else
			{
				red1 = 0;
				green1 = 0;
				blue1 = 0;
			}
		}

		return CRGB(red1, green1, blue1);
	}
#pragma endregion
};
