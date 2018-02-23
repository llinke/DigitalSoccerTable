#include "SerialDebug.h"
#include "FastLedInclude.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
#include <map>

#define DUMP_PALETTE
#define BOOST_SAT_MIN 160
#define BOOST_SAT_INCR 64
#define BOOST_VAL_MIN 128
#define BOOST_VAL_INCR 64

// Helper macro
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

static std::vector<String> CommonColorNames;
static std::map<String, std::vector<CRGB>> CommonColorPalettes;

static uint8_t TeamHueValues[] = {HUE_RED, HUE_YELLOW}; // HUE_GREEN};
static std::map<String, std::vector<CRGB>> TeamColorPalettes[] =
    {
        {
            {"Idle", {CRGB::Black, CRGB(CHSV(TeamHueValues[0], 255, 255)), CRGB::Gray, CRGB(CHSV(TeamHueValues[0], 255, 255))}},
            {"Goal", {CRGB::Black, CRGB::White, CRGB(CHSV(TeamHueValues[0], 255, 255))}},
            {"Celebration", {CRGB(CHSV(TeamHueValues[0], 255, 255)), ((((uint32_t)CRGB(CHSV(TeamHueValues[0], 255, 255))) & 0xFCFCFC) / 4)}},
        },
        {
            {"Idle", {CRGB::Black, CRGB(CHSV(TeamHueValues[1], 255, 255)), CRGB::Gray, CRGB(CHSV(TeamHueValues[1], 255, 255))}},
            {"Goal", {CRGB::Black, CRGB::White, CRGB(CHSV(TeamHueValues[1], 255, 255))}},
            {"Celebration", {CRGB(CHSV(TeamHueValues[1], 255, 255)), ((((uint32_t)CRGB(CHSV(TeamHueValues[1], 255, 255))) & 0xFCFCFC) / 4)}},
        }};

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
        //rgb2hsv_approximate
        std::vector<CRGB> palColorsBoosted;
        CRGB rgbcolor;
        CHSV hsvcolor;
        int colCount = palColors.size();
        for (int c = 0; c < colCount; c++)
        {
            rgbcolor = palColors[c];
            hsvcolor = rgb2hsv_approximate(rgbcolor);
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
            palColorsBoosted.push_back(rgbcolor);
        }
        CommonColorPalettes[palName] = palColorsBoosted;
    }
}

void InitColorPalettes()
{
    DEBUG_PRINTLN("Initializing color palettes...");
    AddColorPalette("Rainbow", {CRGB(0xFF0000), CRGB(0xD52A00), CRGB(0xAB5500), CRGB(0xAB7F00), CRGB(0xABAB00), CRGB(0x56D500), CRGB(0x00FF00), CRGB(0x00D52A), CRGB(0x00AB55), CRGB(0x0056AA), CRGB(0x0000FF), CRGB(0x2A00D5), CRGB(0x5500AB), CRGB(0x7F0081), CRGB(0xAB0055), CRGB(0xD5002B)}, false);
    //AddColorPalette("Idle", {CRGB::Black, CRGB::Gray, CRGB(CHSV(TeamHueValues[0], 255, 255)), CRGB::Gray, CRGB(CHSV(TeamHueValues[1], 255, 255)), CRGB::Gray}, false);
    AddColorPalette("Idle", {CRGB::Black, CRGB::Gray, CRGB(CHSV(TeamHueValues[0], 255, 255)), CRGB::Gray, CRGB(CHSV(TeamHueValues[1], 255, 255)), CRGB::Gray}, false);
}
