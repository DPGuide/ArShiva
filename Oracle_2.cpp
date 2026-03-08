// =========================================================
// Äther-Oracle - Das vereinte Kristall-Orakel
// Kompilieren: g++ -o oracle.exe oracle_2.cpp -mwindows -lcomdlg32 -luser32 -lgdi32
// =========================================================
#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <random>
#include <cmath>
#include <windows.h>
#include "resource.h"
#include <algorithm>

#define ID_BUTTON_ANALYZE 2
#define ID_BUTTON_FIBONACCI 1001
#define ID_TIMER_FIBONACCI 2001
bool g_autoFibonacciActive = false;

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")
// GANZ NACH OBEN DAMIT:
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}
// --- DATENSTRUKTUREN ---
enum WaterState { DEAD_WATER, LIVING_WATER, TRANSITIONING, UNKNOWN };
WaterState AnalyzeWaterState(const std::vector<uint16_t>& data, int& outDeadScore, int& outLivingScore) {
    outDeadScore = 0;
    outLivingScore = 0;

    for (uint16_t glyph : data) {
        if (glyph >= 0x5A5A && glyph <= 0x64BB) {
            outDeadScore++;
        }
        else if (glyph >= 0x79E1 && glyph <= 0x7EF6) {
            outLivingScore++;
        }
    }

    // Wenn AUCH NUR EIN lebendiges Signal da ist, gewinnt das Leben!
    if (outLivingScore > 0) {
        return LIVING_WATER; 
    } 
    else if (outDeadScore > 0) {
        return DEAD_WATER;
    }

    return UNKNOWN;
}
struct CrystalProfile {
    const char* name;
    const char* element;
    const char* property;
    uint16_t resonanceBase;
    std::vector<uint16_t> signaturePattern;
    const char* oracleWisdom[5];
};

struct GlyphInfo {
    const char* category;
    const char* english;
    const char* mystical;
    const char* crystalResonance;
};

struct Signature {
    std::vector<uint16_t> pattern;
    const char* name;
    const char* meaning;
};

// --- DATENBANKEN ---

class OracleDatabase {
public:
    std::vector<CrystalProfile> crystals;
    std::map<uint16_t, GlyphInfo> dictionary;
    std::vector<Signature> signatures;

    OracleDatabase() {
        InitCrystals();
        InitDictionary();
        InitSignatures();
    }

private:
    void InitCrystals() {
        // --- DEIN BESTEHENDER BERGKRISTALL ---
        crystals.push_back({
            "Rock crystal", 
            "Ether", 
            "Clarity, Amplification & Maximum coupling", 
            0x3C00, 
            {0x3CFF}, 
            {
                "The crystal rests in its basic resonance. (0x3C00).",
                "The field achieves maximum coupling (0x3CFF).",
                "The frequency stabilizes within the CJK spectrum..",
                "The electromagnetic field resonates in the ether.."
            }
        });
        // --- NEU: TOTES WASSER (Leitungswasser / Druck) ---
        crystals.push_back({
            "Dead Water (Stagnant)", 
            "Lead / Stagnation", 
            "Rigidity, Pressure & Structural Chaos", 
            0x5A5A, // Deine Untergrenze für totes Wasser
            {0x64BB}, // Deine Obergrenze
            {
                "Water structure is rigid and disconnected (Stagnant Grid).",
                "The flow memory is suppressed by pipe pressure.",
                "Measuring high-frequency dissonance (0x5A5A - 0x64BB).",
                "The element shows signs of energetic stagnation."
            }
        });
        // --- NEU: LEBENDIGES WASSER (Strukturiert / Seide) ---
        crystals.push_back({
            "Living Water (Silk Matrix)", 
            "Life / Vortex", 
            "Coherence, Harmony & Self-Organization", 
            0x79E1, // Der Startwert deines Baches
            {0x7EF6}, // Der Peak deiner "Seiden-Matrix"
            {
                "The silk-thread matrix (U+7EF6) has stabilized.",
                "Harmonic resonance from the forest source detected.",
                "The water has regained its geometric memory.",
                "Abundance completes the circle (0x79E1 - 0x7EF6).",
                "The Silk Spiral is active and vibrant."
            }
        });
    }
// Hilfsfunktion: Sucht den passenden Bild-Encoder (z.B. "image/png")
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

    void InitDictionary() {
    // --- CJK / Kristall-Basis ---
    dictionary[0x3CFF] = {"CJK", "Overflow", "Abundance", "High Resonance"};
    dictionary[0x3C3F] = {"CJK", "Gather", "Convergence", "Focus Point"};
    dictionary[0x3C30] = {"CJK", "Shelter", "Protection", "Stability"};
    dictionary[0x3C00] = {"CJK", "Origin", "Source", "Root Frequency"};
    // --- Tibetan ---
    dictionary[0x0F03] = {"Tibetan", "Sacred Syllable", "Om Energy", "Opening"};
    dictionary[0x0FFC] = {"Tibetan", "Auspicious", "Blessing", "Harmony"};
    dictionary[0x0FCF] = {"Tibetan", "End Mark", "Completion", "Seal"};
    dictionary[0x0F00] = {"Tibetan", "Void", "Emptiness", "Potential"};
    // --- Japanese ---
    dictionary[0x30FF] = {"Japanese", "Sound Wo", "Harmonic Transition", "Bridge"};
    dictionary[0x3042] = {"Japanese", "Hiragana A", "Primary Vibration", "Beginning"};
    dictionary[0x3093] = {"Japanese", "Hiragana N", "Final Resonance", "Closure"};
    // --- Korean ---
    dictionary[0xCFFF] = {"Korean", "Syllable Kweol", "Crystal Resonance", "Peak"};
    dictionary[0xC03F] = {"Korean", "Syllable Kwe", "Attenuated Signal", "Decay"};
    dictionary[0xC000] = {"Korean", "Syllable Kk", "Base Frequency", "Ground"};
    dictionary[0xD7A3] = {"Korean", "Syllable Hih", "Completion", "Finality"};
    // --- Chinese ---
    dictionary[0x4E00] = {"Chinese", "One", "Unity", "Singularity"};
    dictionary[0x9FFF] = {"Chinese", "Complete", "Perfection", "Full Spectrum"};
    dictionary[0x4E8C] = {"Chinese", "Two", "Duality", "Polarity"};
    dictionary[0x4E09] = {"Chinese", "Three", "Trinity", "Manifestation"};
    dictionary[0x56DB] = {"Chinese", "Four", "Stability", "Foundation"};
    dictionary[0x4E94] = {"Chinese", "Five", "Change", "Transformation"};
    dictionary[0x516D] = {"Chinese", "Six", "Harmony", "Balance"};
    dictionary[0x4E03] = {"Chinese", "Seven", "Mystery", "Spirit"};
    dictionary[0x516B] = {"Chinese", "Eight", "Infinity", "Flow"};
    dictionary[0x4E5D] = {"Chinese", "Nine", "Completion", "Fulfillment"};
    dictionary[0x5341] = {"Chinese", "Ten", "Return", "Cycle"};
    // --- Runic (Futhark) ---
    dictionary[0x16A0] = {"Runic", "Fehu / Wealth", "Flowing Energy", "Manifestation"};
    dictionary[0x16A2] = {"Runic", "Uruz / Aurochs", "Primal Power", "Strength"};
    dictionary[0x16A6] = {"Runic", "Thurisaz / Thorn", "Destructive Force", "Catalyst"};
    dictionary[0x16A8] = {"Runic", "Ansuz / God", "Divine Breath", "Communication"};
    dictionary[0x16B1] = {"Runic", "Raido / Journey", "Transformation", "Movement"};
    dictionary[0x16B2] = {"Runic", "Kenaz / Torch", "Illumination", "Knowledge"};
    dictionary[0x16B4] = {"Runic", "Gebo / Gift", "Exchange", "Partnership"};
    dictionary[0x16B7] = {"Runic", "Wunjo / Joy", "Harmony", "Fulfillment"};
    dictionary[0x16B9] = {"Runic", "Hagalaz / Hail", "Disruption", "Testing"};
    dictionary[0x16BB] = {"Runic", "Nauthiz / Need", "Constraint", "Endurance"};
    dictionary[0x16BE] = {"Runic", "Isa / Ice", "Stasis", "Concentration"};
    dictionary[0x16C1] = {"Runic", "Jera / Year", "Harvest", "Cycle"};
    dictionary[0x16C3] = {"Runic", "Eihwaz / Yew", "Endurance", "Defense"};
    dictionary[0x16C7] = {"Runic", "Perthro / Dice Cup", "Fate", "Mystery"};
    dictionary[0x16C8] = {"Runic", "Algiz / Elk", "Protection", "Connection"};
    dictionary[0x16C9] = {"Runic", "Dagaz / Day", "Awakening", "Lightbringer"};
    // --- Celtic (Ogham) ---
    dictionary[0x1680] = {"Celtic", "Space", "Void", "Separator"};
    dictionary[0x1681] = {"Celtic", "Beith / Birch", "New Beginning", "Purification"};
    dictionary[0x1682] = {"Celtic", "Luis / Rowan", "Protection", "Warding"};
    dictionary[0x1683] = {"Celtic", "Fearn / Alder", "Shield", "Endurance"};
    dictionary[0x1684] = {"Celtic", "Saille / Willow", "Intuition", "Dreams"};
    dictionary[0x1685] = {"Celtic", "Nion / Ash", "World Tree", "Connection"};
    dictionary[0x1686] = {"Celtic", "Uath / Hawthorn", "Challenge", "Threshold"};
    dictionary[0x1687] = {"Celtic", "Duir / Oak", "Strength", "Kingship"};
    // --- Cherokee ---
    dictionary[0x13A0] = {"Cherokee", "Syllable A", "Primal Sound", "Base Frequency"};
    dictionary[0x13B2] = {"Cherokee", "Syllable E", "Ascension", "Rising"};
    dictionary[0x13C5] = {"Cherokee", "Syllable I", "Illumination", "Clarity"};
    dictionary[0x13D5] = {"Cherokee", "Syllable O", "Circulation", "Flow"};
    dictionary[0x13E3] = {"Cherokee", "Syllable U", "Depth", "Submersion"};
    // --- Alchemy ---
    dictionary[0x25B2] = {"Alchemy", "Sulfur / Fire", "Flame of Passion", "Transformation"};
    dictionary[0x25BC] = {"Alchemy", "Mercury / Water", "Depth of Ocean", "Emotion"};
    dictionary[0x25C6] = {"Alchemy", "Salt / Earth", "Root of Earth", "Grounding"};
    dictionary[0x25CA] = {"Alchemy", "Air / Spirit", "Essence of Ether", "Freedom"};
    dictionary[0x2605] = {"Alchemy", "Gold Star", "Perfection", "Sublimation"};
    dictionary[0x2625] = {"Alchemy", "Ankh", "Eternal Life", "Regeneration"};
    // --- Astrology ---
    dictionary[0x263F] = {"Astrology", "Mercury", "Cosmic Truth", "Communication"};
    dictionary[0x2640] = {"Astrology", "Venus", "Universal Harmony", "Attraction"};
    dictionary[0x2641] = {"Astrology", "Earth", "Manifestation", "Materialization"};
    dictionary[0x2642] = {"Astrology", "Mars", "Unbridled Will", "Assertion"};
    dictionary[0x2643] = {"Astrology", "Jupiter", "Boundless Abundance", "Expansion"};
    dictionary[0x2644] = {"Astrology", "Saturn", "Eternal Order", "Structure"};
    dictionary[0x2645] = {"Astrology", "Uranus", "Sudden Illumination", "Rebellion"};
    dictionary[0x2646] = {"Astrology", "Neptune", "Veiled Illusion", "Mysticism"};
    dictionary[0x2647] = {"Astrology", "Pluto", "Transforming Rebirth", "Power"};
    dictionary[0x2609] = {"Astrology", "Sun", "Vital Force", "Identity"};
    dictionary[0x263D] = {"Astrology", "Moon", "Reflection", "Emotion"};
    // --- Mystic Symbols ---
    dictionary[0x262F] = {"Mystic", "Yin Yang", "Balance", "Harmony"};
    dictionary[0x2600] = {"Mystic", "Black Sun", "Male Energy", "Active Radiation"};
    dictionary[0x263C] = {"Mystic", "White Sun", "Pure Light", "Clarity"};
    dictionary[0x26A0] = {"Mystic", "Warning", "Disturbance", "Danger"};
    dictionary[0x26A1] = {"Mystic", "Lightning", "Discharge", "Overload"};
    dictionary[0x262A] = {"Mystic", "Star & Crescent", "Nocturnal Revelation", "Dream"};
    dictionary[0x2624] = {"Mystic", "Caduceus", "Perfect Balance", "Healing"};
    dictionary[0x267E] = {"Mystic", "Infinity", "Endless Eternity", "Cycle"};
    dictionary[0x269B] = {"Mystic", "Atom", "Invisible Structure", "Microcosm"};
    dictionary[0x2721] = {"Mystic", "Star of David", "Sacred Union", "Integration"};
    dictionary[0x2638] = {"Mystic", "Wheel of Dharma", "Cosmic Order", "Teaching"};
    dictionary[0x2626] = {"Mystic", "Chi Rho", "Christ Force", "Anointed"};
    dictionary[0x2627] = {"Mystic", "Cross", "Sacrifice", "Redemption"};
    dictionary[0x2628] = {"Mystic", "Cross Orthodox", "Resurrection", "Victory"};
    dictionary[0x2660] = {"Mystic", "Spades (Swords)", "sharp clarity that cuts through the flood"};	
    // --- I Ching ---
    dictionary[0x4DC0] = {"I-Ching", "The Creative", "Primal Force", "Heaven"};
    dictionary[0x4DC1] = {"I-Ching", "The Receptive", "Devotion", "Earth"};
    dictionary[0x4DC2] = {"I-Ching", "Difficulty", "Sprouting", "Challenge"};
    dictionary[0x4DC3] = {"I-Ching", "Youthful Folly", "Inexperience", "Learning"};
    dictionary[0x4DC4] = {"I-Ching", "Waiting", "Nourishment", "Patience"};
    dictionary[0x4DC5] = {"I-Ching", "Conflict", "Strife", "Resolution"};
    dictionary[0x4DD1] = {"I-Ching", "Work on Decay", "Restoration", "Correction"};
    dictionary[0x4DE3] = {"I-Ching", "Darkening Light", "Intelligence Hidden", "Shadow"};
    dictionary[0x4DFF] = {"I-Ching", "Before Completion", "Transition", "Unfinished"};
    // --- Sacred Geometry ---
    dictionary[0x25A0] = {"Geometry", "Black Square", "Manifestation", "Earth"};
    dictionary[0x25B3] = {"Geometry", "Triangle Up", "Ascension", "Fire"};
    dictionary[0x25BD] = {"Geometry", "Triangle Down", "Descent", "Water"};
    dictionary[0x25CB] = {"Geometry", "Circle", "Wholeness", "Spirit"};
    dictionary[0x2B21] = {"Geometry", "Hexagon", "Structure", "Carbon"};
    dictionary[0x2B1F] = {"Geometry", "Pentagon", "Humanity", "Microcosm"};
	// --- Element Water ---
    dictionary[0x5A5A] = {"Element", "Water (Flowing/Pressure)", "falling flood", "life force"};
    // --- Echo/Resonanz from Water ---
    dictionary[0xB4B4] = {"Element", "Water (Deep Echo)", "deep resonance", "river"};
	dictionary[0x2618] = {"Nature", "Shamrock", "the ancient vitality of the earth"};
    dictionary[0x2604] = {"Cosmic", "Comet", "a sudden spark of celestial energy"};
    dictionary[0x263F] = {"Planetary", "Mercury", "the fluid communication of the spirit"};
    dictionary[0x26AB] = {"Balance", "Deep Pool", "the silent wisdom of the deep water"};
    dictionary[0x263C] = {"Light", "Sun", "life-giving warmth and solar power"};
	
}
void InitSignatures() {
    // Original signatures
    signatures.push_back({{0x3CFF, 0x3C3F, 0x0F03, 0x0FFC, 0x0FCF, 0x30FF, 0xCFFF, 0xC03F, 0xC000, 0x3C30, 0x3CFF}, 
        "Crystal Insertion", "Crystal placed in electromagnetic field - resonance begins"});
	signatures.push_back({{0x5A5A, 0x5B5B, 0x5C5C, 0x5A5A, 0x6000, 0x64BB, 0x6000, 0x5A5A, 0x5B5B, 0x5C5C, 0x5A5A}, 
        "Stagnant Grid", "Dead water structure - rigid pipe resonance detected"});
	signatures.push_back({{0x79E1, 0x7A20, 0x7B50, 0x7D80, 0x7EF6, 0x7F00, 0x7EF6, 0x7D80, 0x7B50, 0x7A20, 0x79E1}, 
        "Silk Spiral", "Living water matrix - harmonic silk-thread resonance"});
    signatures.push_back({{0x0F03, 0x0FFC, 0x0FCF}, 
        "Tibetan Trinity", "Spiritual gateway opens"});
    signatures.push_back({{0xCFFF, 0xC03F, 0xC000}, 
        "Korean Energy Decay", "Crystal absorbs and dampens signal"});
    // Runic signatures
    signatures.push_back({{0x16A0, 0x16B1, 0x16C9}, 
        "Nordic Runic Circle", "Protective energetic wall forms in data stream. Strong grounding."});
    signatures.push_back({{0x16A8, 0x16B4, 0x16C8}, 
        "Aesir Blessing", "Divine communication established. Wisdom flows."});
    // Celtic signatures
    signatures.push_back({{0x1681, 0x1682, 0x1685}, 
        "Celtic Tree Binding", "Rooting in ancient memories. Field connects with nature."});
    signatures.push_back({{0x1684, 0x1687}, 
        "Otherworld Gateway", "Threshold to the unseen. Dreams manifest."});
    // Mystic signatures
    signatures.push_back({{0x262F, 0x2600, 0x263D}, 
        "Astral Conjunction", "Perfect balance in ether. Male and female energies aligned."});
    signatures.push_back({{0x13A0, 0x13B2, 0x13E3}, 
        "Cherokee Spirit Call", "Ancient invocation stream. Calls clarity into chaos."});
    signatures.push_back({{0x26A0, 0x26A1}, 
        "Electro-Etheric Discharge", "WARNING: Extreme energy spike. Crystal may be oversaturated."});
    signatures.push_back({{0x25B2, 0x25BC, 0x25C6, 0x25CA}, 
        "Elemental Convergence", "All four elements in resonance. Quintessence forms."});
    signatures.push_back({{0x2640, 0x2642}, 
        "Sacred Marriage", "Coniunctio oppositorum. Divine union of polarities."});
    signatures.push_back({{0x4E00, 0x4E8C, 0x4E09, 0x56DB, 0x4E94}, 
        "Five Phases Flow", "Wu Xing cycle active. Transformation proceeds naturally."});
    signatures.push_back({{0x4DC0, 0x4DC1}, 
        "Heaven-Earth Union", "Tai Chi manifests. Perfect creative balance."});
    signatures.push_back({{0x2625, 0x267E}, 
        "Immortality Sequence", "Ankh and Infinity. Eternal recurrence activated."});
}
};
// --- ANALYSE LOGIK ---
class EtherAnalyzer {
    OracleDatabase db;
    std::mt19937 rng;

public:
    EtherAnalyzer() {
        rng.seed(GetTickCount());
    }

    std::string AnalyzeFullFile(const std::string& filepath) {
        std::ifstream in(filepath, std::ios::binary);
        if (!in) return "Error: File could not be read.";

        std::vector<uint16_t> data;
        char buf[2];
        while (in.read(buf, 2)) {
            uint16_t glyph = (uint8_t)buf[0] | ((uint8_t)buf[1] << 8);
            data.push_back(glyph);
        }
        in.close();

        if (data.empty()) return "Error: The file is empty.";

        return GenerateReport(filepath, data);
    }

    std::string GetCategory(uint16_t character) {
        if (character >= 0x4E00 && character <= 0x9FFF) return "Chinese";
        if ((character >= 0x3040 && character <= 0x30FF) || 
            (character >= 0x31F0 && character <= 0x31FF) || 
            (character >= 0xFF65 && character <= 0xFF9F)) return "Japanese";
        if ((character >= 0xAC00 && character <= 0xD7AF) || 
            (character >= 0x1100 && character <= 0x11FF) || 
            (character >= 0x3130 && character <= 0x318F)) return "Korean";
        if (character >= 0x0F00 && character <= 0x0FFF) return "Tibetan";
        if (character >= 0x16A0 && character <= 0x16FF) return "Runic";
        if (character >= 0x1680 && character <= 0x169F) return "Celtic (Ogham)";
        if (character >= 0x2600 && character <= 0x26FF) return "Mystic";
        if (character >= 0x13A0 && character <= 0x13FF) return "Cherokee";
        if (character >= 0x2800 && character <= 0x28FF) return "Braille";
        if (character >= 0x2700 && character <= 0x27BF) return "Dingbats";
        if (character >= 0x3400 && character <= 0x4DBF) return "CJK Ext A";
        if (character >= 0x3100 && character <= 0x312F) return "Bopomofo";

        auto it = db.dictionary.find(character);
        if (it != db.dictionary.end()) return it->second.category;

        return ""; 
    }
	std::string GenerateAetherMandala(const std::vector<uint16_t>& data, const std::string& originalFilename) {
    using namespace Gdiplus;

    if (data.size() < 10) return "";

    int width = 1000;
    int height = 1000;
    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // Erstelle ein unsichtbares Canvas im RAM
    Bitmap bmp(width, height, PixelFormat32bppARGB);
    Graphics g(&bmp);

    // Ganz wichtig für fließende, mystische Linien: Anti-Aliasing aktivieren!
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    // Hintergrund: Das gleiche mystische Dunkelgrau wie dein UI
    g.Clear(Color(255, 15, 15, 20));

    // Pinsel definieren: Ein halbtransparentes Cyan für Rauschen, Gold für Bergkristall
    Pen cyanPen(Color(100, 0, 220, 255), 1.5f); 
    Pen goldPen(Color(200, 255, 215, 0), 2.0f); 

    // Wir berechnen die Punkte für EINEN Strang des Mandalas
    std::vector<PointF> basePoints;
    
    // Wir nehmen z.B. die ersten 720 Datenpunkte (für eine schöne Dichte)
    size_t pointsToUse = std::min((size_t)720, data.size());
    
    for (size_t i = 0; i < pointsToUse; i++) {
        // Wir ziehen die 2-Bit Struktur heraus, um die "Ebene" zu bestimmen
        int state = (data[i] >> 6) & 0x03; // Gibt 0, 1, 2 oder 3
        int rawVariance = data[i] & 0xFF;  // Für ein bisschen natürliches Chaos

        // Radius-Berechnung (Entfernung vom Zentrum)
        // State 0 ist innen, State 3 schlägt weit nach außen aus
        float radius = 50.0f + (state * 100.0f) + (rawVariance / 2.0f);

        // Winkel-Berechnung: Wir wickeln die Zeitachse als Spirale auf
        // 3 Umdrehungen verteilt auf die verfügbaren Punkte
        float angleDeg = (i * (360.0f * 3.0f / pointsToUse)); 
        float angleRad = angleDeg * (3.14159265f / 180.0f); // Umrechnung in Bogenmaß

        float px = cx + radius * cos(angleRad);
        float py = cy + radius * sin(angleRad);
        
        basePoints.push_back(PointF(px, py));
    }

    // === HIER ENTSTEHT DIE MAGIE DER SYMMETRIE ===
    // Wir zeichnen den Strang 8 Mal, jedes Mal um 45 Grad gedreht
    for (int sym = 0; sym < 8; sym++) {
        g.ResetTransform();
        // GDI+ Rotation funktioniert vom Nullpunkt (0,0), also verschieben wir das Zentrum
        g.TranslateTransform(cx, cy);
        g.RotateTransform(sym * 45.0f); // 8-fache Symmetrie (360 / 8 = 45)
        g.TranslateTransform(-cx, -cy);
        for (size_t i = 1; i < basePoints.size(); i++) {
            // Wenn der Datenpunkt im Bergkristall-Fenster (0x3C00) liegt, leuchtet er Gold!
            if (data[i] >= 0x3C00 && data[i] <= 0x3CFF) {
                g.DrawLine(&goldPen, basePoints[i-1], basePoints[i]);
            } else {
                g.DrawLine(&cyanPen, basePoints[i-1], basePoints[i]);
            }
        }
    }
	std::string outPath = originalFilename + "_mandala.png";
    CLSID pngClsid;
    if (GetEncoderClsid(L"image/png", &pngClsid) != -1) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &outPath[0], (int)outPath.size(), NULL, 0);
        std::wstring wOutPath(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &outPath[0], (int)outPath.size(), &wOutPath[0], sizeNeeded);

        bmp.Save(wOutPath.c_str(), &pngClsid, NULL);
    }
	return outPath;
}

private:
    std::string GenerateReport(const std::string& filename, const std::vector<uint16_t>& data) {
        std::stringstream report;
        const CrystalProfile* crystal = IdentifyCrystal(data);

        report << "╔══════════════════════════════════════════════════╗\r\n";
        report << "║           ETHER ORACLE COMPLETE ANALYSIS         ║\r\n";
        report << "╚══════════════════════════════════════════════════╝\r\n\r\n";
        
        report << "📄 FILE: " << filename << "\r\n";
        report << "   Characters read: " << data.size() << " (" << data.size() * 2 << " Bytes)\r\n\r\n";

        // 1. Kristall Identifikation (mit Check auf echte Daten)
        if (crystal != nullptr) {
            report << "🔮 DOMINANT CRYSTAL: " << crystal->name << "\r\n";
            report << "   Element: " << crystal->element << "\r\n";
            report << "   Characteristic: " << crystal->property << "\r\n";
            report << "   Measuring range: 0x3C00 (Basis) to 0x3CFF (Peak)\r\n\r\n";

            std::uniform_int_distribution<int> dist(0, 4);
            report << "📜 STATUS:\r\n";
            report << "   \"" << crystal->oracleWisdom[dist(rng)] << "\"\r\n\r\n";
        } else {
            report << "🔮 DOMINANT CRYSTAL: Unknown\r\n";
            report << "   (No resonances could be detected in the known band 0x3C00 - 0x3CFF be measured.)\r\n\r\n";
        }
		// 2.1 Wasser-Matrix Analyse
		// Wir bereiten zwei leere Variablen vor, die die Funktion für uns füllen soll
		int deadScore = 0;
		int livingScore = 0;

		// Jetzt rufen wir die Funktion mit allen DREI Argumenten auf:
		WaterState waterState = AnalyzeWaterState(data, deadScore, livingScore); 

		// Und hier geben wir das Wasser-Ergebnis aus (inklusive der neuen Debug-Zeile!)
		report << "🌊 WATER MATRIX ANALYSIS:\r\n";
		report << "   [DEBUG] Dead Hits: " << deadScore << " | Living Hits: " << livingScore << "\r\n";
		report << "🌊 WATER MATRIX ANALYSIS:\r\n";
		if (waterState == LIVING_WATER) {
			report << "   Status: LIVING WATER (Silk Matrix)\r\n";
			report << "   Resonance: High Frequency (0x79E1 to 0x7EF6)\r\n";
			report << "   Characteristic: Coherence, Harmony & Self-Organization\r\n\r\n";
			report << "📜 ALCHEMICAL FINDING:\r\n";
			report << "   \"The water has regained its geometric memory. The Silk Spiral is active.\"\r\n\r\n";
		} 
		else if (waterState == DEAD_WATER) {
			report << "   Status: DEAD WATER (Stagnant Grid)\r\n";
			report << "   Resonance: Low Frequency Dissonance (0x5A5A to 0x64BB)\r\n";
			report << "   Characteristic: Rigidity, Pressure & Structural Chaos\r\n\r\n";
			report << "📜 ALCHEMICAL FINDING:\r\n";
			report << "   \"Water structure is rigid. The flow memory is suppressed by pipe pressure.\"\r\n\r\n";
		} 
		else if (waterState == TRANSITIONING) {
			report << "   Status: TRANSITIONING (Phase Shift)\r\n";
			report << "   Resonance: Mixed Frequencies / High Chaos\r\n";
			report << "   Characteristic: Breakdown of old structures\r\n\r\n";
			report << "📜 ALCHEMICAL FINDING:\r\n";
			report << "   \"Turbulence detected. The old matrix is breaking down to form anew.\"\r\n\r\n";
		} 
		else {
			report << "   Status: UNKNOWN / NEUTRAL\r\n";
			report << "   (No clear water signature or structure could be detected.)\r\n\r\n";
		}
        // 2.2 Energetische Analyse
        report << "📊 ENERGETIC ANALYSIS:\r\n";
        double avg = 0, variance = 0;
        for (auto g : data) avg += g;
        avg /= data.size();
        for (auto g : data) variance += pow(g - avg, 2);
        variance /= data.size();
        double chaos = sqrt(variance);

        report << "   Average response: U+" << std::hex << std::uppercase << (int)avg << "\r\n";
        report << "   Chaos-Faktor: " << std::dec << std::fixed << std::setprecision(1) << chaos << "\r\n";
        
        if (chaos < 1000) report << "   -> Very stable, meditative state\r\n";
        else if (chaos < 5000) report << "   -> Balanced, clear flow\r\n";
        else if (chaos < 10000) report << "   -> Dynamic, transformative energy\r\n";
        else report << "   -> Turbulent, major change is imminent\r\n\r\n";

        // --- Regionale und Mystische Verteilung (Dynamisch) ---
        std::map<std::string, int> categoryCounts;
        for (auto g : data) {
            std::string cat = GetCategory(g);
            if (!cat.empty()) {
                categoryCounts[cat]++; 
            }
        }
        
        report << "   Spiritual Signatures (filtered):\r\n";
        if (categoryCounts.empty()) {
            report << "   • No significant patterns were found.\r\n";
        } else {
            for (const auto& pair : categoryCounts) {
                report << "   • " << pair.first << ": " << pair.second << " Resonances\r\n";
            }
        }
        report << "\r\n";

        // 3. Bekannte Muster suchen
        auto sigs = FindSignatures(data);
        report << "🔍 FOUND PATTERNS (SIGNATURES):\r\n";
        if (sigs.empty()) {
            report << "   No known closed patterns found.\r\n\r\n";
        } else {
            for (const auto& [pos, sig] : sigs) {
                report << "   position " << pos << ": " << sig->name << "\r\n";
                report << "   Meaning: " << sig->meaning << "\r\n\r\n";
            }
        }

        // 4. Detail-Übersetzung (Original mit Erweiterungen)
        report << "=== DETAILED TRANSLATION (Original Timeline) ===\r\n";
        
        uint16_t lastValue = 0;
        int validGlyphsFound = 0;            // <--- WICHTIG: Definition wieder da!
        std::vector<std::string> poemWords;  // <--- WICHTIG: Definition wieder da!
        
        for (size_t i = 0; i < data.size(); i++) {
            std::string cat = GetCategory(data[i]);
            bool isMystic = (data[i] >= 0x2600 && data[i] <= 0x26FF);
            
            if (!cat.empty() && (data[i] != lastValue || isMystic)) { 
                
                if (isMystic) {
                    report << "\r\n[!!!] MYSTISCHE RESONANZ: U+" 
                           << std::hex << std::uppercase << std::setw(4) << data[i] << " [!!!]\r\n";
                }

                report << "[Position " << std::setw(6) << std::setfill('0') << std::dec << i << "] U+" 
                       << std::hex << std::uppercase << std::setw(4) << data[i]
                       << " | [" << cat << "]";
                
                auto it = db.dictionary.find(data[i]);
                if (it != db.dictionary.end()) {
                    // Check ob Wort schon im Topf ist
                    if (std::find(poemWords.begin(), poemWords.end(), it->second.mystical) == poemWords.end()) {
                        poemWords.push_back(it->second.mystical);
                    }
                    report << " ---> " << it->second.mystical; 
                }
                
                report << "\r\n";
                lastValue = data[i]; 
                validGlyphsFound++;
            }
        }
		// --- 5. DAS MYSTISCHE GEDICHT (Flexibel und unzerstörbar) ---
        report << "\r\n╔══════════════════════════════════════════════════╗\r\n";
        report << "║               THE SONG OF THE Aether             ║\r\n";
        report << "╚══════════════════════════════════════════════════╝\r\n";
        
        if (!poemWords.empty()) {
            // Wir mischen die gefundenen Wörter
            std::shuffle(poemWords.begin(), poemWords.end(), rng);
            
            // Wir nutzen den Modulo-Trick (%), damit das Programm nie abstürzt, 
            // egal wie viele Wörter wir gefunden haben!
            size_t n = poemWords.size();
            
            report << "   Hidden in the noise, the " << poemWords[0] << ",\r\n";
            report << "   led by " << poemWords[1 % n] << " through time and space.\r\n";
            report << "   The field whispers of " << poemWords[2 % n] << ",\r\n";
            
            // Wenn wir unser seltenes drittes Wort haben, kriegt es einen Ehrenplatz:
            if (n >= 3) {
                report << "   until " << poemWords[n-1] << " completes the circle.\r\n\r\n";
            } else {
                report << "   until the echo completes the circle.\r\n\r\n";
            }
            if (n >= 1) {
                report << "   Do not search in the light, but in the " << poemWords[0] << ",\r\n";
                report << "   because there awaits the " << poemWords[1 % n] << ".\r\n\r\n";
            }
        } else {
            report << "   The field was too quiet. The echoes weren't strong enough,\r\n";
            report << "   to form a clear song from the ether.\r\n\r\n";
        }
        if (validGlyphsFound == 0) {
            report << "No known categories or dictionary entries found (only background noise).\r\n";
        }
        return report.str();
    }
    const CrystalProfile* IdentifyCrystal(const std::vector<uint16_t>& data) {
        int bergkristallScore = 0;
        for (uint16_t glyph : data) {
            // Echte Signatur prüfen: Bereich 0x3C00 bis 0x3CFF
            if (glyph >= 0x3C00 && glyph <= 0x3CFF) {
                bergkristallScore++;
            }
        }
        // Wenn Signale in diesem Frequenzband gefunden wurden, ist es der Bergkristall
        if (bergkristallScore > 0 && !db.crystals.empty()) {
            return &db.crystals[0]; // Bergkristall
        }
        // Wenn nichts im Bereich 0x3C00-0x3CFF liegt, kennen wir den Kristall (noch) nicht
        return nullptr; 
    }
    std::vector<std::pair<size_t, const Signature*>> FindSignatures(const std::vector<uint16_t>& data) {
        std::vector<std::pair<size_t, const Signature*>> results;
        for (const auto& sig : db.signatures) {
            if (sig.pattern.size() > data.size()) continue;
            for (size_t i = 0; i <= data.size() - sig.pattern.size(); ++i) {
                bool match = true;
                for (size_t j = 0; j < sig.pattern.size(); ++j) {
                    if (data[i + j] != sig.pattern[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) results.push_back(std::make_pair(i, &sig));
            }
        }
        return results;
    }
};
// --- GLOBALE VARIABLEN ---
HWND hEdit, hStatus, hBtnAnalyze, hBtnBrowse, hBtnMandala;
char g_filename[260] = {0};
HFONT hFont;
EtherAnalyzer analyzer;
// Grafik-Variablen
HBITMAP g_hbgImage = NULL;
HBRUSH g_hbrEditBkgnd = NULL;
HICON g_hIcon = NULL;
// --- HILFSFUNKTION FÜR UNICODE ---
std::wstring UTF8ToWide(const std::string& utf8Str) {
    if (utf8Str.empty()) return L"";
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), NULL, 0);
    std::wstring wideStr(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), &wideStr[0], sizeNeeded);
    return wideStr;
}
// --- FUNKTIONEN ---
void BrowseFile(HWND hwnd) {
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = g_filename;
    ofn.nMaxFile = sizeof(g_filename);
    ofn.lpstrFilter = "Raw/Text Files\0*.raw;*.txt\0All Files\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn)) {
        SetWindowTextA(hStatus, g_filename);
    }
}
void AnalyzeFile() {
    if (strlen(g_filename) == 0) {
        SetWindowTextW(hStatus, UTF8ToWide("Please select a file first!").c_str());
        return;
    }
    SetWindowTextW(hStatus, UTF8ToWide("Analyzing ether currents... Please wait.").c_str());
    // Analyse durchführen
    std::string result = analyzer.AnalyzeFullFile(g_filename);
    std::wstring wResult = UTF8ToWide(result);
    SetWindowTextW(hEdit, wResult.c_str());
    std::string outFile = std::string(g_filename) + "_oracle_translation.txt";
    std::ofstream out(outFile);
    out << result;
    out.close();
    std::string statusMsg = "Done. Saved as: " + outFile;
    SetWindowTextA(hStatus, statusMsg.c_str());
}
void PlayFibonacciResonance() {
    int a = 1;
    int b = 1;
    int next;
    int baseFreq = 528; //528 Hz
    int duration = 150; //150 Millisekunden
    int multiplier = 100; //Pausen in Millisekunden
    //ersten 10 Fibonacci-Stufen
    for(int i = 0; i < 10; i++) {
        Beep(baseFreq, duration);
        Sleep(a * multiplier);
        // 3. Berechne die nächste Fibonacci-Zahl (1, 1, 2, 3, 5, 8...)
        next = a + b;
        a = b;
        b = next;
    }
}
// --- WINDOW PROCEDURE ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // 1. Icon Laden & Erstes Button
            CreateWindow("BUTTON", "Fibonacci Rythm", 
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
                         10, 100, 150, 30, // x, y, Breite, Höhe
                         hwnd, (HMENU)ID_BUTTON_FIBONACCI, NULL, NULL);
            g_hIcon = (HICON)LoadImageA(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
            if (g_hIcon) {
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_hIcon);
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hIcon);
            }
            g_hbgImage = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MYBG));
            //2. Pinsel für Textfeld
            g_hbrEditBkgnd = CreateSolidBrush(RGB(15, 15, 20));
            hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
            // UI-Elemente mit absolut sicheren Hex-Codes für Umlaute (Ö = \x00D6, ä = \x00E4)
            hBtnBrowse = CreateWindowExW(0, L"BUTTON", L"OPEN \x00D6" L"FILE...",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                20, 20, 150, 35, hwnd, (HMENU)1, NULL, NULL);
            SendMessage(hBtnBrowse, WM_SETFONT, (WPARAM)hFont, TRUE);
            hBtnAnalyze = CreateWindowExW(0, L"BUTTON", L"consult an oracle",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                180, 20, 150, 35, hwnd, (HMENU)2, NULL, NULL);
            SendMessage(hBtnAnalyze, WM_SETFONT, (WPARAM)hFont, TRUE);
            hBtnMandala = CreateWindowExW(0, L"BUTTON", L"DRAW MANDALA",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                340, 20, 150, 35, hwnd, (HMENU)3, NULL, NULL);
            SendMessage(hBtnMandala, WM_SETFONT, (WPARAM)hFont, TRUE);
            hStatus = CreateWindowExW(0, L"STATIC", L"Ready. Please attach a file. w\x00E4" L"hlen.",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                20, 65, 740, 20, hwnd, NULL, NULL, NULL);
            SendMessage(hStatus, WM_SETFONT, (WPARAM)hFont, TRUE);
            hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                20, 200, 740, 390, hwnd, NULL, NULL, NULL);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            return 0;
        }
        case WM_ERASEBKGND: {
            if (g_hbgImage) {
                HDC hdc = (HDC)wParam;
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbgImage);
                BITMAP bm;
                GetObject(g_hbgImage, sizeof(bm), &bm);
                RECT rc;
                GetClientRect(hwnd, &rc);
                SetStretchBltMode(hdc, HALFTONE);
                SetBrushOrgEx(hdc, 0, 0, NULL);
                StretchBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);
                return 1; 
            }
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            HWND hwndCtrl = (HWND)lParam; 
            // 1. Ist es unser großes Orakel-Textfeld?
            if (hwndCtrl == hEdit) {
                SetTextColor(hdcStatic, RGB(0, 220, 255)); // Mystisches Cyan
                SetBkColor(hdcStatic, RGB(15, 15, 20));    // Muss exakt wie g_hbrEditBkgnd sein!
                SetBkMode(hdcStatic, OPAQUE);              // OPAQUE beendet den Geister-Schmier-Effekt!
                return (LRESULT)g_hbrEditBkgnd;
            } 
            else {
                SetTextColor(hdcStatic, RGB(255, 215, 0)); // Gold / Gelb
                SetBkMode(hdcStatic, TRANSPARENT);         // Der darf transparent über dem Tempel schweben
                return (LRESULT)GetStockObject(NULL_BRUSH);
            }
        }
        case WM_CTLCOLOREDIT: {
            HDC hdcEdit = (HDC)wParam;
            SetTextColor(hdcEdit, RGB(0, 220, 255)); 
            SetBkColor(hdcEdit, RGB(15, 15, 20)); 
            SetBkMode(hdcEdit, OPAQUE);
            return (LRESULT)g_hbrEditBkgnd;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) BrowseFile(hwnd);
            if (LOWORD(wParam) == 2) AnalyzeFile();
            if (LOWORD(wParam) == 3) {
                if (strlen(g_filename) == 0) {
                    SetWindowTextW(hStatus, L"Please select a file first!");
                    break;
                }
                SetWindowTextW(hStatus, L"Drawing Aether Mandala... Please wait.");
                std::ifstream in(g_filename, std::ios::binary);
                if (!in) break;
                std::vector<uint16_t> data;
                char buf[2];
                while (in.read(buf, 2)) {
                    data.push_back((uint8_t)buf[0] | ((uint8_t)buf[1] << 8));
                }
                in.close();
                if (!data.empty()) {
                    std::string savedPath = analyzer.GenerateAetherMandala(data, g_filename);
                    if (!savedPath.empty()) {
                        SetWindowTextA(hStatus, ("Mandala rendered! Opened: " + savedPath).c_str());
                        ShellExecuteA(NULL, "open", savedPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    }
                }
            }
            if (LOWORD(wParam) == ID_BUTTON_FIBONACCI) {
				if (!g_autoFibonacciActive) {
					// AKTIVIEREN
					g_autoFibonacciActive = true;
					// Timer starten: Alle 60000 Millisekunden (60 Sekunden)
					SetTimer(hwnd, ID_TIMER_FIBONACCI, 60000, NULL);
            
					// Text auf dem Button ändern, damit man sieht, dass es läuft
					SetWindowText((HWND)lParam, "Auto-Fib: ON");
            
					// Einmal sofort abspielen, damit man nicht 60 Sek warten muss
					SendMessage(hwnd, WM_TIMER, ID_TIMER_FIBONACCI, 0);
				} else {
					// DEAKTIVIEREN
					g_autoFibonacciActive = false;
					KillTimer(hwnd, ID_TIMER_FIBONACCI);
					SetWindowText((HWND)lParam, "Fibonacci Rythm");
					MessageBox(hwnd, "Automatic stopped.", "Aether Oracle", MB_OK);
				}
			}
			break;
        }
		case WM_TIMER:
			if (wParam == ID_TIMER_FIBONACCI) {
				// --- Hier ist dein Fibonacci-Rhythmus ---
				int a = 1, b = 1, next;
				int baseFreq = 528;
        
				for(int i = 0; i < 12; i++) {
					Beep(baseFreq, 200); 
					Sleep(a * 50);
					next = a + b;
					a = b;
					b = next;
					if(a > 100) { a = 1; b = 1; } 
				}
			}
			break;
        case WM_DESTROY: {
            DeleteObject(hFont);
            if (g_hbgImage) DeleteObject(g_hbgImage);
            if (g_hbrEditBkgnd) DeleteObject(g_hbrEditBkgnd);
            if (g_hIcon) DestroyIcon(g_hIcon);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
// --- ENTRY POINT ---
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    HICON hIconBig   = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 64, 64, 0);
    HICON hIconSmall = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);
    WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"EtherOracleClass";
    wc.hIcon   = hIconBig;
    wc.hIconSm = hIconSmall;
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowExW(0, L"EtherOracleClass", L"\x00C4" L"ther-Oracle - Master Decoder \xD83D\xDD2E",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, hInst, NULL);
		// --- GDI+ ENGINE HOCHFAHREN ---
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}