#pragma once
#include <unit_audioplayer.hpp>
#include "./GlobalParentClass.h"

// ─── Krem Renk Paleti ─────────────────────────────────────────
#define AP_COLOR_BG       0x0000
#define AP_COLOR_CREAM1   0xF7D6   // Ana krem (açık)
#define AP_COLOR_CREAM2   0xDECB   // Orta krem
#define AP_COLOR_CREAM3   0xC5A3   // Koyu krem / sepia
#define AP_COLOR_CREAM4   0x2945   // Çok koyu kahve (panel bg)
#define AP_COLOR_CREAM5   0x18C3   // Derin kahverengi
#define AP_COLOR_AMBER    0xFD20   // Amber / turuncu vurgu
#define AP_COLOR_WARM     0xFC80   // Sıcak sarı
#define AP_COLOR_GREEN    0x67E0   // Soluk yeşil (VU yeşil)
#define AP_COLOR_ORANGE   0xFD20   // Turuncu (VU orta)
#define AP_COLOR_RED      0xF800   // Kırmızı (VU peak)
#define AP_COLOR_WHITE    0xFFFF   // Beyaz

// ─── Layout Sabitleri ─────────────────────────────────────────
#define LEFT_W    130
#define DISK_CX   184
#define DISK_CY    66
#define DISK_R     42

// ─── Ana Sınıf ────────────────────────────────────────────────
class UnitAudioPlayerController : public GlobalParentClass {

public:
    // Yaşam döngüsü
    void Begin();
    void Loop();
    void Draw();
    UnitAudioPlayerController(MyOS *os) : GlobalParentClass(os) {}

private:
    // ── Çekirdek ──────────────────────────────────────────────
    AudioPlayerUnit audioplayer;
    bool            initialized = false;

    // ── Parça / Çalma Durumu ──────────────────────────────────
    uint8_t  lastPlayStatus     = AUDIO_PLAYER_STATUS_STOPPED;
    uint16_t currentTrack       = 1;
    uint16_t lastDisplayedTrack = 1;
    uint16_t totalTracks        = 0;

    unsigned long trackStartTime  = 0;
    unsigned long pausedTime      = 0;
    unsigned long totalPausedTime = 0;
    bool          isPlaying       = false;

    // ── Mod Bayrakları ────────────────────────────────────────
    bool isLoopEnabled    = false;
    bool isShuffleEnabled = false;

    // ── Ses ───────────────────────────────────────────────────
    int currentVolume = 25;

    // ── Ekran Durumu ──────────────────────────────────────────
    bool isInfoScreen = false;

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
    //  Yardımcı / Çizim Metodları (private)
    // ─────────────────────────────────────────────────────────

    // Genel çizim araçları
    void drawCreamLine(int y, float brightness = 0.9f);

    // Ana ekran bileşenleri
    void drawMainScreen();
    void drawTopBar();
    void drawLeftPanel();
    void drawBottomBar();

    // Disk
    void drawDiskBackground();
    void drawDiskDynamic(float angle);
    void drawFullDisk(float angle);
    void updateDiskRotation();

    // VU Metre
    void updateVuMeter();
    void drawVuMeter();

    // Güncelleme (kısmi yeniden çizim)
    void updateMainDisplay();
    void updateTrackDisplay();
    void updateStatusDisplay();
    void updateModeDisplay();
    void updateTimeDisplay();

    // Geçici mesaj
    void showTempMsg(const String& msg, uint16_t col);
    void drawTempMsg();
    void clearTempArea();

    // Info ekranı
    void enterInfoScreen();
    void exitInfoScreen();

    // Kontrol / Eylemler
    void volumeUp();
    void volumeDown();
    void toggleLoop();
    void toggleShuffle();
    void goToTrack(int n);
    void randomTrack();
    void executeAction(int clicks);
    void checkAutoNext();

    // Klavye / Buton işleme
    void handleKeyboard();
    void handleButtonA();
};
