#include "SerialDebug.h"
#include "FastLedInclude.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
#include <map>

#define DUMP_PALETTE

//#define BOOST_COLORS
#define BOOST_SAT_MIN 160
#define BOOST_SAT_INCR 16
#define BOOST_VAL_MIN 128
#define BOOST_VAL_INCR 16

// Helper macro
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

static std::vector<String> CommonColorNames;
static std::map<String, std::vector<CRGB>> CommonColorPalettes;

const CRGB LED_BLACK = 0x000000;
const CRGB LED_GRAY_DARK = 0x101010;
const CRGB LED_GRAY_MEDIUM = 0x202020;
const CRGB LED_GRAY_LIGHT = 0x404040;
const CRGB LED_WHITE = 0xFFFFFF;

const std::vector<String> BundesligaTeamKeys = {
    // "TEST1", "TEST2",
    "BVB",
    "FCB",
    "BMG",
    "WB",
    "RBL",
    "BSC",
    "FFM",
    "TSG",
    "FCA",
    "VFL",
    "SCF",
    "B04",
    "M05",
    "FCN",
    "S04",
    "H96",
    "F95",
    "VFB",
    "1FCK",
    "SCP",
    "ABI",
    "HSV",
    "FL13",
    "ESC",
    "FCRB"};

const std::map<String, String> BundesligaTeamNames =
    {
        // {"TEST1", "Test 1"},
        // {"TEST2", "Test 2"},
        //{"BVB", "Borussia Dortmund"},
        {"BVB", "B.Dortmund"},
        {"FCB", "B.München"},
        //{"BMG", "Borussia Mönchengladbach"},
        {"BMG", "M'gladbach"},
        {"WB", "W.Bremen"},
        {"RBL", "RB Leipzig"},
        {"BSC", "Herta BSC"},
        //{"FFM", "Eintracht Frankfurt"},
        {"FFM", "E.Frankfurt"},
        //{"TSG", "TSG Hoffenheim"},
        {"TSG", "Hoffenheim"},
        //{"FCA", "FC Augsburg"},
        {"FCA", "Augsburg"},
        {"VFL", "Wolfsburg"},
        {"SCF", "SC Freiburg"},
        //{"B04", "Bayer 04 Leverkusen"},
        {"B04", "Leverkusen"},
        {"M05", "Mainz 05"},
        {"FCN", "Nürnberg"},
        {"S04", "Schalke 04"},
        {"H96", "Hannover 96"},
        //{"F95", "Fortuna Düsseldorf"},
        {"F95", "F.Düsseldorf"},
        //{"VFB", "VfB Stuttgard"},
        {"VFB", "Stuttgard"},
        {"1FCK", "1.FC Köln"},
        //{"SCP", "SC Paderborn"},
        {"SCP", "SC Paderb."},
        //{"ABI", "Arminia Bielefeld"},
        {"ABI", "A.Bielefeld"},
        //{"HSV", "Hamburg HSV"},
        {"HSV", "Hamburg"},
        {"FL13", "Feenland 13"},
        {"ESC", "SC Einhorn"},
        {"FCRB", "FC Rainbow"}};

const std::map<String, std::vector<CRGB>> BundesligaTeamColors =
    {
        // {"TEST1", {0xFFFFFF, 0x808080, 0x404040}},
        // {"TEST2", {0x404040, 0x202020, 0x101010}},
        {"BVB", {0xFEE101, 0xFEE101, 0xFEE101, 0xFEE101, 0x404040, 0x404040}},
        {"FCB", {0xC80024, 0xC80024, 0xE2032F, 0xE2032F, 0x404040, 0x404040}},
        {"BMG", {0x007F4C, 0x007F4C, 0x01AB7C, 0x01AB7C, 0xFFFFFF, 0xFFFFFF}},
        {"WB", {0x008B6C, 0x008B6C, 0x008B6C, 0x008B6C, 0xFFFFFF, 0xFFFFFF}},
        {"RBL", {0xFFFFFF, 0xFFFFFF, 0xBA1205, 0xBA1205, 0xFDE500, 0xFDE500}},
        {"BSC", {0x3C7AE7, 0x3C7AE7, 0x1445AB, 0x1445AB, 0xFFFFFF, 0xFFFFFF}},
        {"FFM", {0x5E5E5E, 0x5E5E5E, 0x5E5E5E, 0x5E5E5E, 0xE00211, 0xE00211}},
        {"TSG", {0x0188E4, 0x0188E4, 0x0052AA, 0x0052AA, 0xFFFFFF, 0xFFFFFF}},
        {"FCA", {0xFFFFFF, 0xFFFFFF, 0xBA3733, 0xBA3733, 0x0EAC58, 0x0EAC58}},
        {"VFL", {0x1C9E75, 0x1C9E75, 0x487F7F, 0x487F7F, 0x1E3636, 0x1E3636}},
        {"SCF", {0xE64B49, 0xE64B49, 0xCC1C27, 0xCC1C27, 0x404040, 0x404040}},
        {"B04", {0x404040, 0x404040, 0xA0171E, 0xA0171E, 0xE24745, 0xE24745}},
        {"M05", {0xAC2A28, 0xAC2A28, 0x842022, 0x842022, 0xFFFFFF, 0xFFFFFF}},
        {"FCN", {0x770C18, 0x770C18, 0xD63C46, 0xD63C46, 0xFFFFFF, 0xFFFFFF}},
        {"S04", {0x1666C9, 0x1666C9, 0x0F2684, 0x0F2684, 0xFFFFFF, 0xFFFFFF}},
        {"H96", {0x73111A, 0x73111A, 0xA63139, 0xA63139, 0xFFFFFF, 0xFFFFFF}},
        {"F95", {0xE62625, 0xE62625, 0x720D15, 0x720D15, 0xFFFFFF, 0xFFFFFF}},
        {"VFB", {0xE34039, 0xE34039, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}},
        {"1FCK", {0xE52941, 0xE52941, 0xE52941, 0xE52941, 0xFFFFFF, 0xFFFFFF}},
        {"SCP", {0x0D68C3, 0x0D68C3, 0x404040, 0x404040, 0x404040, 0x404040}},
        {"ABI", {0x174CBE, 0x174CBE, 0x404040, 0x404040, 0xFFFFFF, 0xFFFFFF}},
        {"HSV", {0x138EAD, 0x138EAD, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}},
        {"FL13", {0xFA96B2, 0xFA96B2, 0x8F4F82, 0x8F4F82, 0x9E7396, 0x9E7396}},
        {"ESC", {0xD90B7C, 0xD90B7C, 0xFDBC0C, 0xFDBC0C, 0x13C5AB, 0x13C5AB}},
        {"FCRB", {0xDF151A, 0xFD8603, 0xF4F328, 0x00DA3C, 0x00CBE7, 0xAA00FF}}};

static uint8_t BundesligaTeamValues[] = {0, 1}; // BVB FCB
//static String BundesligaTeamLabels[] = {"", ""};

static std::map<String, std::vector<CRGB>> TeamColorPalettes[] =
    {
        {
            {"Idle", {}},
            {"KickOff", {}},
            {"InGame", {}},
            {"Goal", {}},
            {"Goal2", {}},
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
            DEBUG_PRINT("Boost: Keeping 0x");
            PrintHex8(rgbcolor.r);
            PrintHex8(rgbcolor.g);
            PrintHex8(rgbcolor.b);
            DEBUG_PRINTLN(", pure grey/white.");
        }
        else
        {
            DEBUG_PRINT("Boost: Boosting 0x");
            PrintHex8(rgbcolor.r);
            PrintHex8(rgbcolor.g);
            PrintHex8(rgbcolor.b);
            DEBUG_PRINT(",");
            DEBUG_PRINT(" S:");
            DEBUG_PRINT(hsvcolor.s);
            if (hsvcolor.s < BOOST_SAT_MIN)
            {
                DEBUG_PRINT("->");
                while (hsvcolor.s < BOOST_SAT_MIN)
                    hsvcolor.s += BOOST_SAT_INCR;
                DEBUG_PRINT(hsvcolor.s);
            }
            DEBUG_PRINT(", V:");
            DEBUG_PRINT(hsvcolor.v);
            if (hsvcolor.v < BOOST_VAL_MIN)
            {
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
        DEBUG_PRINT("Boost: Keeping 0x");
        PrintHex8(rgbcolor.r);
        PrintHex8(rgbcolor.g);
        PrintHex8(rgbcolor.b);
        uint8_t minSat = BOOST_SAT_MIN;
        uint8_t minVal = BOOST_VAL_MIN;
        DEBUG_PRINTLN(", S:" + String(hsvcolor.s) + ">" + String(minSat) + " and V:" + String(hsvcolor.v) + ">" + String(minVal) + ".");
    }
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
    DEBUG_PRINTLN("Initializing team " + String(teamNr + 1) + "...");
    String teamKey = BundesligaTeamKeys.at(BundesligaTeamValues[teamNr]);

    DEBUG_PRINTLN("Team:" + teamKey + " -> " + BundesligaTeamNames.find(teamKey)->second);
    //BundesligaTeamLabels[teamNr] = BundesligaTeamNames.find(teamKey)->second;

    std::vector<CRGB> teamColors = BundesligaTeamColors.find(teamKey)->second;
    TeamColorPalettes[teamNr]["Idle"] = //AnalogousPaletteFromHue(TeamHueValues[teamNr]);
        {
            LED_BLACK,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5)),
            LED_GRAY_MEDIUM,
            LED_BLACK,
            LED_GRAY_MEDIUM,
            BoostColor(teamColors.at(5)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(0))};
    TeamColorPalettes[teamNr]["KickOff"] =
        {
            LED_BLACK,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(0)),
            LED_BLACK,
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            LED_BLACK,
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5)),
            LED_BLACK,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5)),
            LED_BLACK};
    TeamColorPalettes[teamNr]["InGame"] =
        {
            LED_BLACK,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5)),
            LED_GRAY_MEDIUM,
            LED_BLACK,
            LED_GRAY_MEDIUM,
            BoostColor(teamColors.at(5)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(0))};
    TeamColorPalettes[teamNr]["Goal"] =
        {
            LED_BLACK,
            LED_GRAY_MEDIUM,
            LED_GRAY_MEDIUM,
            LED_WHITE,
            LED_WHITE,
            LED_WHITE,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5))};
    TeamColorPalettes[teamNr]["Goal2"] =
        {
            BoostColor(teamColors.at(5)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(0)),
            LED_GRAY_MEDIUM,
            LED_GRAY_DARK,
            LED_GRAY_MEDIUM,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5)),
            LED_GRAY_MEDIUM,
            LED_BLACK};
    TeamColorPalettes[teamNr]["Celebration"] =
        {
            // LED_BLACK,
            LED_GRAY_DARK,
            BoostColor(teamColors.at(0)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(5)),
            LED_GRAY_MEDIUM,
            LED_GRAY_DARK,
            LED_GRAY_MEDIUM,
            BoostColor(teamColors.at(5)),
            BoostColor(teamColors.at(4)),
            BoostColor(teamColors.at(3)),
            BoostColor(teamColors.at(2)),
            BoostColor(teamColors.at(1)),
            BoostColor(teamColors.at(0))};
}

void InitColorPalettes()
{
    DEBUG_PRINTLN("Initializing common color palettes...");
    AddColorPalette("Rainbow", {CRGB(0xFF0000), CRGB(0xD52A00), CRGB(0xAB5500), CRGB(0xAB7F00), CRGB(0xABAB00), CRGB(0x56D500), CRGB(0x00FF00), CRGB(0x00D52A), CRGB(0x00AB55), CRGB(0x0056AA), CRGB(0x0000FF), CRGB(0x2A00D5), CRGB(0x5500AB), CRGB(0x7F0081), CRGB(0xAB0055), CRGB(0xD5002B)}, false);

    CreateTeamColorPalettes(0);
    CreateTeamColorPalettes(1);
}
