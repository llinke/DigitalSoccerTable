#include "SerialDebug.h"
#include "FastLedInclude.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
#include <map>

#define DUMP_PALETTE

#define BOOST_COLORS
#define BOOST_SAT_MIN 160
#define BOOST_SAT_INCR 64
#define BOOST_VAL_MIN 128
#define BOOST_VAL_INCR 32

#define HUE_VARIANT 16

// Helper macro
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

static std::vector<String> CommonColorNames;
static std::map<String, std::vector<CRGB>> CommonColorPalettes;

static uint8_t TeamHueValues[] = {HUE_RED, HUE_YELLOW};
static std::map<String, std::vector<CRGB>> TeamColorPalettes[] =
    {
        {
            {"Idle", {}},
            {"KickOff", {}},
            {"InGame", {}},
            {"Goal", {}},
            {"Celebration", {}},
        },
        {
            {"Idle", {}},
            {"KickOff", {}},
            {"InGame", {}},
            {"Goal", {}},
            {"Goal2", {}},
            {"Celebration", {}},
        }};

#pragma region Helper Methods
void PrintHex8(uint8_t data) // prints 8-bit data in hex with leading zeroes
{
    if (data < 0x10)
        Serial.print("0");
    DEBUG_PRINT(data, HEX);
}

void DumpPalette(std::vector<CRGB> pal)
{
    DEBUG_PRINT("Colors: ");
    CRGB rgbcolor;
    for (int c = 0; c < pal.size(); c++)
    {
        rgbcolor = pal.at(c);
        DEBUG_PRINT("0x");
        PrintHex8(rgbcolor.r);
        PrintHex8(rgbcolor.g);
        PrintHex8(rgbcolor.b);
        if (c != pal.size() - 1)
        {
            DEBUG_PRINT(",");
        }
    }
    DEBUG_PRINTLN(".");
}

CRGB BoostColor(CRGB origrgbcolor)
{
    CRGB rgbcolor = origrgbcolor;
#ifdef BOOST_COLORS
    CHSV hsvcolor = rgb2hsv_approximate(rgbcolor);
    if (hsvcolor.s < BOOST_SAT_MIN || hsvcolor.v < BOOST_VAL_MIN)
    {
        if (rgbcolor.r == rgbcolor.g && rgbcolor.g == rgbcolor.b)
        {
            DEBUG_PRINT("   Keeping 0x");
            PrintHex8(rgbcolor.r);
            PrintHex8(rgbcolor.g);
            PrintHex8(rgbcolor.b);
            DEBUG_PRINTLN(", pure grey/white.");
        }
        else
        {
            DEBUG_PRINT("   Boosting 0x");
            PrintHex8(rgbcolor.r);
            PrintHex8(rgbcolor.g);
            PrintHex8(rgbcolor.b);
            DEBUG_PRINT(",");
            if (hsvcolor.s < BOOST_SAT_MIN)
            {
                DEBUG_PRINT(" S:");
                DEBUG_PRINT(hsvcolor.s);
                DEBUG_PRINT("->");
                while (hsvcolor.s < BOOST_SAT_MIN)
                    hsvcolor.s += BOOST_SAT_INCR;
                DEBUG_PRINT(hsvcolor.s);
            }
            if (hsvcolor.v < BOOST_VAL_MIN)
            {
                DEBUG_PRINT(" V:");
                DEBUG_PRINT(hsvcolor.v);
                DEBUG_PRINT("->");
                while (hsvcolor.v < BOOST_VAL_MIN)
                    hsvcolor.v += BOOST_VAL_INCR;
                DEBUG_PRINT(hsvcolor.v);
            }
            rgbcolor = CRGB(hsvcolor);
            DEBUG_PRINT(", now 0x");
            PrintHex8(rgbcolor.r);
            PrintHex8(rgbcolor.g);
            PrintHex8(rgbcolor.b);
            DEBUG_PRINTLN(".");
        }
    }
    else
    {
        DEBUG_PRINT("   Keeping 0x");
        PrintHex8(rgbcolor.r);
        PrintHex8(rgbcolor.g);
        PrintHex8(rgbcolor.b);
        uint8_t minSat = BOOST_SAT_MIN;
        uint8_t minVal = BOOST_VAL_MIN;
        DEBUG_PRINTLN(", S:" + String(hsvcolor.s) + ">" + String(minSat) + " and V:" + String(hsvcolor.v) + ">" + String(minVal) + ".");
    }
    // rgbcolor.r = (rgbcolor.r & 0xfe) >> 1;
    // rgbcolor.g = (rgbcolor.g & 0xfe) >> 1;
    // rgbcolor.b = (rgbcolor.b & 0xfe) >> 1;
    // DEBUG_PRINT("   Final: 0x");
    // PrintHex8(rgbcolor.r);
    // PrintHex8(rgbcolor.g);
    // PrintHex8(rgbcolor.b);
    // DEBUG_PRINTLN(".");
#endif
    return rgbcolor;
}

void AddColorPalette(
    String palName,
    std::vector<CRGB> palColors,
    bool boostSaturation = false)
{
    DEBUG_PRINT("ColorPalettes: adding color palette '");
    DEBUG_PRINT(palName);
    DEBUG_PRINTLN("'.");
    CommonColorNames.push_back(palName);
    if (!boostSaturation)
    {
        CommonColorPalettes[palName] = palColors;
    }
    else
    {
        std::vector<CRGB> palColorsBoosted;
        CRGB rgbcolor;
        int colCount = palColors.size();
        for (int c = 0; c < colCount; c++)
        {
            rgbcolor = BoostColor(palColors[c]);
            palColorsBoosted.push_back(rgbcolor);
        }
        CommonColorPalettes[palName] = palColorsBoosted;
    }
}

std::vector<CRGB> SimplePaletteFromColor(
    CRGB baseCol, int variants = 1, uint8_t variantBy = 32)
{
    std::vector<CRGB> newPal;
    CRGB rgb = CRGB(baseCol);
    newPal.push_back(rgb);
    for (int i = 0; i < variants; i++)
    {
        CHSV hsv = rgb2hsv_approximate(rgb);
        hsv.h += 32;
        rgb = CRGB(hsv);
        newPal.push_back(rgb);
    }
    return newPal;
}

std::vector<CRGB> AdjacentPaletteFromHue(
    uint8_t hue, bool addCompl = false, uint8_t distance = 30)
{
    uint8_t dist8 = (255 * distance) / 360;
    std::vector<CRGB> newPal;
    CHSV hsvBase = CHSV(hue, 255, 255);
    newPal.push_back(CRGB(hsvBase)); // base color
    CHSV hsvNew = CHSV(hue + dist8, 255, 255);
    newPal.push_back(CRGB(hsvNew)); // right adjacent
    if (addCompl)
    {
        hsvNew = CHSV(hue - 128, 255, 255);
        newPal.push_back(CRGB(hsvNew)); // complementary
    }
    else
    {
        newPal.push_back(CRGB(hsvBase)); // base color
    }
    hsvNew = CHSV(hue - dist8, 255, 255);
    newPal.push_back(CRGB(hsvNew)); // left adjacent
    return newPal;
}

std::vector<CRGB> TriadPaletteFromHue(
    uint8_t hue, bool addCompl = false, uint8_t distance = 30)
{
    return AdjacentPaletteFromHue(hue, distance + 90, addCompl);
}

std::vector<CRGB> AnalogousPaletteFromHue(
    uint8_t hue, bool addCompl = false, uint8_t distance = 15)
{
    uint8_t dist8 = (255 * distance) / 360;
    std::vector<CRGB> newPal;
    CHSV hsvBase = CHSV(hue, 255, 255);
    newPal.push_back(CRGB(hsvBase)); // base color
    CHSV hsvNew = CHSV(hue - dist8, 255, 255);
    newPal.push_back(CRGB(hsvNew)); // first left analogous
    hsvNew = CHSV(hue - 2 * dist8, 255, 255);
    newPal.push_back(CRGB(hsvNew)); // second left analogous
    if (addCompl)
    {
        hsvNew = CHSV(hue - 128, 255, 255);
        newPal.push_back(CRGB(hsvNew)); // complementary
    }
    else
    {
        newPal.push_back(CRGB(hsvBase)); // base color
    }
    hsvNew = CHSV(hue + 2 * dist8, 255, 255);
    newPal.push_back(CRGB(hsvNew)); // second right analogous
    hsvNew = CHSV(hue + dist8, 255, 255);
    newPal.push_back(CRGB(hsvNew)); // first right analogous
    if (addCompl)
    {
        // stronger base color
        newPal.push_back(CRGB(hsvBase)); // base color
    }
    return newPal;
}

#pragma endregion

static void CreateTeamColorPalettes(int teamNr)
{
    DEBUG_PRINTLN("Initializing team " + String(teamNr + 1) + "'s color palettes...");
    TeamColorPalettes[teamNr]["Idle"] = //AnalogousPaletteFromHue(TeamHueValues[teamNr]);
        {
            0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr] - HUE_VARIANT, 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x808080, // Medium Gray
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr] + HUE_VARIANT, 255, 128)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255))};
    TeamColorPalettes[teamNr]["KickOff"] =
        {
            0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x000000, // Black
            0x000000, // Black
            0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x000000, // Black
            0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x000000};
    TeamColorPalettes[teamNr]["InGame"] =
        {
            0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr] - HUE_VARIANT, 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x808080, // Medium Gray
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x808080, // Medium Gray
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr] + HUE_VARIANT, 255, 128)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255))};
    TeamColorPalettes[teamNr]["Goal"] =
        {
            0x000000, // Black
            0xFFFFFF, // White
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255))};
    TeamColorPalettes[teamNr]["Goal2"] =
        {
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255 + HUE_VARIANT, 255)),
            0x808080, // Medium Gray
            CRGB(CHSV(TeamHueValues[teamNr], 255 - HUE_VARIANT, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255))};
    TeamColorPalettes[teamNr]["Celebration"] =
        {
            // 0x000000, // Black
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x808080, // Medium Gray
            CRGB(CHSV(TeamHueValues[teamNr] + HUE_VARIANT, 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr] + HUE_VARIANT, 255, 255)),
            0x404040, // Dark Gray
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            0x404040, // Dark Gray
            CRGB(CHSV(TeamHueValues[teamNr] - HUE_VARIANT, 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255)),
            CRGB(CHSV(TeamHueValues[teamNr] - HUE_VARIANT, 255, 255)),
            0x808080, // Medium Gray
            CRGB(CHSV(TeamHueValues[teamNr], 255, 255))};
}

void InitColorPalettes()
{
    DEBUG_PRINTLN("Initializing common color palettes...");
    AddColorPalette("Rainbow", {CRGB(0xFF0000), CRGB(0xD52A00), CRGB(0xAB5500), CRGB(0xAB7F00), CRGB(0xABAB00), CRGB(0x56D500), CRGB(0x00FF00), CRGB(0x00D52A), CRGB(0x00AB55), CRGB(0x0056AA), CRGB(0x0000FF), CRGB(0x2A00D5), CRGB(0x5500AB), CRGB(0x7F0081), CRGB(0xAB0055), CRGB(0xD5002B)}, false);

    CreateTeamColorPalettes(0);
    CreateTeamColorPalettes(1);
}
