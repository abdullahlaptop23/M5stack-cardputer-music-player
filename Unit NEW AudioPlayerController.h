#ifndef UNITAUDIOPLAYERCONTROLLER_H
#define UNITAUDIOPLAYERCONTROLLER_H

#include "MenuClass.h"
#include <unit_audioplayer.hpp>

class UnitAudioPlayerController : public MenuClass
{
private:
    // Audio Player
    AudioPlayerUnit audioplayer;
    
    // Track bilgileri
    uint16_t currentTrack = 1;
    uint16_t lastDisplayedTrack = 1;
    uint16_t totalTracks = 0;
    uint8_t lastPlayStatus = AUDIO_PLAYER_STATUS_STOPPED;
    
    // Timing
    unsigned long trackStartTime = 0;
    unsigned long pausedTime = 0;
    unsigned long totalPausedTime = 0;
    unsigned long lastTimeUpdate = 0;
    unsigned long lastDiskUpdate = 0;
    unsigned long lastVuUpdate = 0;
    unsigned long tempMessageTime = 0;
    unsigned long lastClickTime = 0;
    
    const unsigned long timeUpdateInterval = 1000;
    const unsigned long diskUpdateInterval = 50;
    const unsigned long vuUpdateInterval = 80;
    const unsigned long tempMessageDuration = 2000;
    const unsigned long clickTimeout = 500;
    
    // State
    bool isPlaying = false;
    bool isLoopEnabled = false;
    bool isShuffleEnabled = false;
    bool tempMessageActive = false;
    bool isInfoScreen = false;
    bool lastDiskPlaying = false;
    
    // Volume
    int currentVolume = 25;
    
    // Disk animasyon
    float diskAngle = 0.0f;
    float diskSpeed = 0.0f;
    float targetDiskSpeed = 0.0f;
    
    // VU Meter
    int vuLeft[12] = {0};
    int vuRight[12] = {0};
    
    // Temp mesaj
    String tempMessageText = "";
    uint16_t tempMessageColor = 0xFFFF;
    
    // Click sayacı
    int clickCount = 0;
    
    // Renkler - Krem Paleti
    const uint16_t COLOR_BG = 0x0000;
    const uint16_t COLOR_CREAM1 = 0xF7D6;
    const uint16_t COLOR_CREAM2 = 0xDECB;
    const uint16_t COLOR_CREAM3 = 0xC5A3;
    const uint16_t COLOR_CREAM4 = 0x2945;
    const uint16_t COLOR_CREAM5 = 0x18C3;
    const uint16_t COLOR_AMBER = 0xFD20;
    const uint16_t COLOR_WARM = 0xFC80;
    const uint16_t COLOR_GREEN = 0x67E0;
    const uint16_t COLOR_ORANGE = 0xFD20;
    const uint16_t COLOR_RED = 0xF800;
    const uint16_t COLOR_WHITE = 0xFFFF;
    
    // Layout
    const int LEFT_W = 130;
    const int DISK_CX = 184;
    const int DISK_CY = 66;
    const int DISK_R = 42;
    
    // Çizim fonksiyonları
    void drawCreamLine(int y, float br = 0.9f);
    void drawDiskBackground();
    void drawDiskDynamic(float angle);
    void drawFullDisk(float angle);
    void updateDiskRotation();
    void updateVuMeter();
    void drawVuMeter();
    void drawLeftPanel();
    void drawBottomBar();
    void drawTopBar();
    void drawMainScreen();
    void drawTempMsg();
    void clearTempArea();
    void showTempMsg(String msg, uint16_t col);
    
    // Güncelleme fonksiyonları
    void updateTrackDisplay();
    void updateStatusDisplay();
    void updateModeDisplay();
    void updateTimeDisplay();
    void updateMainDisplay();
    
    // Info ekranı
    void enterInfoScreen();
    void exitInfoScreen();
    
    // Kontrol fonksiyonları
    void volumeUp();
    void volumeDown();
    void toggleLoop();
    void toggleShuffle();
    void randomTrack();
    void goToTrack(int n);
    void executeAction(int clicks);
    void checkAutoNext();
    
public:
    UnitAudioPlayerController(MyOS* osPtr) : MenuClass(osPtr) {}
    void Begin() override;
    void Loop() override;
    void Draw() override;
};

#endif
