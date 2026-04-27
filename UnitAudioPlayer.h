#pragma once
#include <unit_audioplayer.hpp>
#include "./GlobalParentClass.h"

// ─── Krem Renk Paleti ─────────────────────────────────────────
#define AP_COLOR_BG       0x0000
#define AP_COLOR_CREAM1   0xF7D6
#define AP_COLOR_CREAM2   0xDECB
#define AP_COLOR_CREAM3   0xC5A3
#define AP_COLOR_CREAM4   0x2945
#define AP_COLOR_CREAM5   0x18C3
#define AP_COLOR_AMBER    0xFD20
#define AP_COLOR_WARM     0xFC80
#define AP_COLOR_GREEN    0x67E0
#define AP_COLOR_ORANGE   0xFD20
#define AP_COLOR_RED      0xF800
#define AP_COLOR_WHITE    0xFFFF

// ─── Layout Sabitleri ─────────────────────────────────────────
#define LEFT_W    130
#define DISK_CX   184
#define DISK_CY    66
#define DISK_R     42

// ═════════════════════════════════════════════════════════════
//  GLOBAL PLAYBACK STATE  (menüler arası taşınan durum)
// ═════════════════════════════════════════════════════════════
struct AudioPlaybackState {
    bool     active          = false;   // modül başlatıldı mı?
    bool     isPlaying       = false;
    uint16_t currentTrack    = 1;
    uint16_t totalTracks     = 0;
    int      currentVolume   = 25;
    bool     isLoopEnabled   = false;
    bool     isShuffleEnabled= false;
    uint8_t  lastPlayStatus  = 0;      // AUDIO_PLAYER_STATUS_*

    unsigned long trackStartTime  = 0;
    unsigned long pausedTime      = 0;
    unsigned long totalPausedTime = 0;
};

// Tek global örnek – tüm Translation Unit'lerde paylaşılır
extern AudioPlaybackState g_audioState;
// Modül nesnesi de global tutulmalı ki Serial1 bağlantısı korunsun
extern AudioPlayerUnit    g_audioPlayer;

// ─── Ana Sınıf ────────────────────────────────────────────────
class UnitAudioPlayerController : public GlobalParentClass {

public:
    void Begin();
    void Loop();
    void Draw();
    UnitAudioPlayerController(MyOS *os) : GlobalParentClass(os) {}

private:
    // ── Ekran / UI Durumu (sadece bu instance'a ait) ──────────
    bool isInfoScreen  = false;
    bool isInputMode   = false;
    String inputTrackNumber = "";

    // ── Disk Animasyonu ───────────────────────────────────────
    float         diskAngle       = 0.0f;
    float         diskSpeed       = 0.0f;
    float         targetDiskSpeed = 0.0f;
    bool          lastDiskPlaying = false;
    unsigned long lastDiskUpdate  = 0;
    static const unsigned long DISK_UPDATE_MS = 50;

    // ── VU Metre ──────────────────────────────────────────────
    int           vuLeft[12]   = {0};
    int           vuRight[12]  = {0};
    unsigned long lastVuUpdate = 0;
    static const unsigned long VU_UPDATE_MS = 80;

    // ── Zamanlayıcılar ────────────────────────────────────────
    unsigned long lastTimeUpdate = 0;
    static const unsigned long TIME_UPDATE_MS = 1000;

    // ── Geçici Mesaj ──────────────────────────────────────────
    bool          tempMessageActive = false;
    unsigned long tempMessageTime   = 0;
    String        tempMessageText   = "";
    uint16_t      tempMessageColor  = 0xFFFF;
    static const unsigned long TEMP_MSG_DURATION = 2000;

    // ── Buton Çoklu Tıklama ───────────────────────────────────
    unsigned long lastClickTime = 0;
    int           clickCount    = 0;
    static const unsigned long CLICK_TIMEOUT = 500;

    // ─────────────────────────────────────────────────────────
    //  Çizim Metodları
    // ─────────────────────────────────────────────────────────
    void drawCreamLine(int y, float brightness = 0.9f,
                       float offset = 0.0f);
    void drawRainbowLine(int y, float br = 0.9f, float off = 0.0f);
    void drawNeonText(int x, int y, const char* txt,
                      uint16_t col, uint8_t sz = 1);
    void drawGlowRect(int x, int y, int w, int h,
                      uint16_t col, uint16_t inner);

    void drawMainScreen();
    void drawTopBar();
    void drawLeftPanel();
    void drawBottomBar();

    void drawDiskBackground();
    void drawDiskDynamic(float angle);
    void drawFullDisk(float angle);
    void updateDiskRotation();

    void updateVuMeter();
    void drawVuMeter();

    void updateMainDisplay();
    void updateTrackDisplay();
    void updateStatusDisplay();
    void updateModeDisplay();
    void updateTimeDisplay();
    void updateBatteryDisplay();

    void showTempMsg(const String& msg, uint16_t col);
    void drawTempMsg();
    void clearTempArea();

    void enterInfoScreen();
    void exitInfoScreen();

    void drawInputMode();

    // ── Kontrol ───────────────────────────────────────────────
    void volumeUp();
    void volumeDown();
    void toggleLoop();
    void toggleShuffle();
    void goToTrack(int n);
    void randomTrack();
    void executeAction(int clicks);
    void checkAutoNext();

    void handleKeyboard();
    void handleKeyboardNormal();
    void handleKeyboardInput();
    void handleButtonA();

    // ── Çıkış Diyaloğu ───────────────────────────────────────
    void handleExitRequest();

    // ── Visualizer stubs ─────────────────────────────────────
    void enterFullscreenVisualizer();
    void exitFullscreenVisualizer();
    void switchVisualizerMode();
    void handleKeyboardVisualizer();
    void updateFullscreenVisualizer();
    void drawFullscreenBars();
    void drawFullscreenWaves();
    void drawMatrixRain();
    void drawVisualizerHeader();
    void initMatrixColumns();
    void drawFullscreenTempMessage(const String&, uint16_t);
    void showSplash();
    uint16_t hsvToRgb565(float, float, float);

    static const char matrixChars[];
};
