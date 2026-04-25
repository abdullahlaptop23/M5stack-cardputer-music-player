#include "UnitAudioPlayerController.h"
#include <M5Cardputer.h>
#include "../ExtraMenu.h"

// ========== YARDIMCI FONKSİYONLAR ==========

void UnitAudioPlayerController::drawCreamLine(int y, float br) {
    for (int x = 0; x < 240; x++) {
        float t = (float)x / 239.0f;
        uint8_t rv = (uint8_t)(31 * br * (0.5f + 0.5f * t));
        uint8_t gv = (uint8_t)(28 * br * (0.4f + 0.6f * t));
        uint8_t bv = (uint8_t)(10 * br * t);
        M5.Display.drawPixel(x, y, M5.Display.color565(rv, gv, bv));
    }
}

// ========== VİNİL DİSK ==========

void UnitAudioPlayerController::drawDiskBackground() {
    // Dış glow
    for (int r = DISK_R + 4; r > DISK_R; r--) {
        float br = 0.10f + 0.10f * (DISK_R + 4 - r);
        uint8_t rv = (uint8_t)(20 * br);
        uint8_t gv = (uint8_t)(15 * br);
        M5.Display.drawCircle(DISK_CX, DISK_CY, r,
            M5.Display.color565(rv, gv, 0));
    }
    // Ana disk
    M5.Display.fillCircle(DISK_CX, DISK_CY, DISK_R, COLOR_CREAM5);
    // Yivler
    int rr[] = {38, 34, 30, 26, 22, 18, 14};
    for (int i = 0; i < 7; i++) {
        uint8_t rv = (uint8_t)(6 + i * 5);
        uint8_t gv = (uint8_t)(4 + i * 4);
        M5.Display.drawCircle(DISK_CX, DISK_CY, rr[i],
            M5.Display.color565(rv, gv, 0));
    }
}

void UnitAudioPlayerController::drawDiskDynamic(float angle) {
    // Dönen renkli halka
    for (int r = DISK_R - 1; r > DISK_R - 7; r--) {
        for (int a = 0; a < 360; a += 3) {
            float rad = a * 3.14159f / 180.0f;
            float rot = fmod((float)a / 360.0f + angle / 360.0f, 1.0f);
            uint8_t rv = (uint8_t)(15 + rot * 16);
            uint8_t gv = (uint8_t)(10 + rot * 12);
            int px = DISK_CX + (int)(r * cosf(rad));
            int py = DISK_CY + (int)(r * sinf(rad));
            if (px >= LEFT_W && px < 240 && py >= 18 && py < 118)
                M5.Display.drawPixel(px, py, M5.Display.color565(rv, gv, 0));
        }
    }
    
    // Dönen parlak nokta
    static float lastSA = 0;
    {
        float lr = lastSA * 3.14159f / 180.0f;
        int lx = DISK_CX + (int)((DISK_R - 8) * cosf(lr));
        int ly = DISK_CY + (int)((DISK_R - 8) * sinf(lr));
        M5.Display.fillCircle(lx, ly, 3, COLOR_CREAM5);
    }
    lastSA = angle;
    float rad = angle * 3.14159f / 180.0f;
    int sx = DISK_CX + (int)((DISK_R - 8) * cosf(rad));
    int sy = DISK_CY + (int)((DISK_R - 8) * sinf(rad));
    M5.Display.fillCircle(sx, sy, 3, COLOR_CREAM1);
    M5.Display.fillCircle(sx, sy, 1, COLOR_AMBER);
    
    // Merkez etiket
    M5.Display.fillCircle(DISK_CX, DISK_CY, 14, COLOR_CREAM5);
    if (isPlaying) {
        float t = fmod(angle / 360.0f, 1.0f);
        uint8_t rv = (uint8_t)(16 + t * 15);
        uint8_t gv = (uint8_t)(10 + t * 10);
        for (int r2 = 14; r2 >= 12; r2--)
            M5.Display.drawCircle(DISK_CX, DISK_CY, r2,
                M5.Display.color565(rv, gv, 0));
        M5.Display.fillCircle(DISK_CX, DISK_CY, 11, COLOR_BG);
        M5.Display.fillCircle(DISK_CX, DISK_CY, 8, COLOR_CREAM2);
        M5.Display.fillCircle(DISK_CX, DISK_CY, 5, COLOR_BG);
        M5.Display.fillCircle(DISK_CX, DISK_CY, 3, COLOR_CREAM3);
    } else {
        M5.Display.drawCircle(DISK_CX, DISK_CY, 13, 0x4208);
        M5.Display.fillCircle(DISK_CX, DISK_CY, 11, 0x2945);
        M5.Display.fillCircle(DISK_CX, DISK_CY, 7, 0x4208);
        M5.Display.fillCircle(DISK_CX, DISK_CY, 4, COLOR_CREAM5);
    }
    M5.Display.fillCircle(DISK_CX, DISK_CY, 2, COLOR_BG);
    M5.Display.drawCircle(DISK_CX, DISK_CY, 2, 0x8410);
}

void UnitAudioPlayerController::drawFullDisk(float angle) {
    M5.Display.fillRect(LEFT_W, 18, 240 - LEFT_W, 100, COLOR_BG);
    drawDiskBackground();
    drawDiskDynamic(angle);
}

void UnitAudioPlayerController::updateDiskRotation() {
    targetDiskSpeed = isPlaying ? 5.0f : 0.0f;
    if (diskSpeed < targetDiskSpeed) {
        diskSpeed += 0.3f;
        if (diskSpeed > targetDiskSpeed) diskSpeed = targetDiskSpeed;
    } else if (diskSpeed > targetDiskSpeed) {
        diskSpeed -= 0.15f;
        if (diskSpeed < 0) diskSpeed = 0;
    }
    diskAngle = fmod(diskAngle + diskSpeed, 360.0f);
}

// ========== VU METER ==========

void UnitAudioPlayerController::updateVuMeter() {
    if (!isPlaying) {
        for (int i = 0; i < 12; i++) {
            if (vuLeft[i] > 0) vuLeft[i]--;
            if (vuRight[i] > 0) vuRight[i]--;
        }
        return;
    }
    int peak = random(5, 12);
    for (int i = 0; i < 12; i++) 
        vuLeft[i] = (i < peak) ? 1 : max(0, vuLeft[i] - 1);
    peak = random(4, 11);
    for (int i = 0; i < 12; i++) 
        vuRight[i] = (i < peak) ? 1 : max(0, vuRight[i] - 1);
}

void UnitAudioPlayerController::drawVuMeter() {
    int bW = 7, bH = 5, bG = 1, sx = 4, lyY = 82, ryY = 90;
    for (int i = 0; i < 12; i++) {
        uint16_t on_c, off_c;
        if (i < 6) { 
            on_c = COLOR_CREAM2; off_c = COLOR_CREAM5; 
        } else if (i < 9) { 
            on_c = COLOR_AMBER; off_c = COLOR_CREAM5; 
        } else if (i < 11) { 
            on_c = COLOR_ORANGE; off_c = COLOR_CREAM5; 
        } else { 
            on_c = COLOR_RED; off_c = COLOR_CREAM5; 
        }
        
        M5.Display.fillRect(sx + i * (bW + bG), lyY, bW, bH,
            vuLeft[i] ? on_c : off_c);
        if (vuLeft[i])
            M5.Display.drawFastHLine(sx + i * (bW + bG), lyY, bW, COLOR_WHITE);
            
        M5.Display.fillRect(sx + i * (bW + bG), ryY, bW, bH,
            vuRight[i] ? on_c : off_c);
        if (vuRight[i])
            M5.Display.drawFastHLine(sx + i * (bW + bG), ryY, bW, COLOR_WHITE);
    }
}

// ========== SOL PANEL ==========

void UnitAudioPlayerController::drawLeftPanel() {
    M5.Display.fillRect(0, 18, LEFT_W, 100, COLOR_BG);
    
    // TRACK HEADER
    M5.Display.drawFastHLine(0, 18, LEFT_W, COLOR_CREAM2);
    M5.Display.fillRect(0, 19, LEFT_W, 16, COLOR_CREAM5);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_CREAM5);
    M5.Display.setCursor(4, 23);
    M5.Display.print("\x0E TRACK");
    char tot[12]; 
    sprintf(tot, "/%d", totalTracks);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_CREAM5);
    M5.Display.setCursor(82, 23);
    M5.Display.print(tot);
    M5.Display.drawFastHLine(0, 35, LEFT_W, COLOR_CREAM3);
    
    // TRACK NUMARASI
    M5.Display.fillRect(0, 36, LEFT_W, 28, COLOR_BG);
    char ts[6]; 
    sprintf(ts, "%03d", currentTrack);
    uint16_t numCol = isPlaying ? COLOR_CREAM1 : COLOR_CREAM2;
    M5.Display.setTextSize(3);
    if (isPlaying) {
        M5.Display.setTextColor(M5.Display.color565(12, 8, 0), COLOR_BG);
        M5.Display.setCursor(5, 39);
        M5.Display.print(ts);
        M5.Display.setTextColor(M5.Display.color565(20, 15, 4), COLOR_BG);
        M5.Display.setCursor(4, 38);
        M5.Display.print(ts);
    }
    M5.Display.setTextColor(numCol, COLOR_BG);
    M5.Display.setCursor(4, 37);
    M5.Display.print(ts);
    
    // TIME
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.setCursor(76, 37);
    M5.Display.print("TIME");
    unsigned long el = isPlaying ?
        (millis() - trackStartTime - totalPausedTime) / 1000 : 0;
    M5.Display.setTextColor(COLOR_WHITE, COLOR_BG);
    M5.Display.setCursor(74, 47);
    M5.Display.printf("%02d:%02d", (int)(el / 60), (int)(el % 60));
    
    M5.Display.drawFastHLine(0, 64, LEFT_W, 0x4208);
    
    // STATUS
    M5.Display.fillRect(0, 65, LEFT_W, 15, COLOR_BG);
    if (isPlaying) {
        M5.Display.fillRoundRect(2, 66, 66, 12, 2,
            M5.Display.color565(20, 14, 2));
        M5.Display.drawRoundRect(2, 66, 66, 12, 2, COLOR_CREAM1);
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(COLOR_CREAM1,
            M5.Display.color565(20, 14, 2));
        M5.Display.setCursor(6, 69);
        M5.Display.print(">> PLAYING");
    } else {
        M5.Display.fillRoundRect(2, 66, 66, 12, 2,
            M5.Display.color565(10, 8, 1));
        M5.Display.drawRoundRect(2, 66, 66, 12, 2, COLOR_CREAM3);
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(COLOR_CREAM2,
            M5.Display.color565(10, 8, 1));
        M5.Display.setCursor(6, 69);
        M5.Display.print("|| PAUSED");
    }
    
    // MOD
    M5.Display.fillRect(72, 65, LEFT_W - 72, 15, COLOR_BG);
    if (isLoopEnabled) {
        M5.Display.fillRoundRect(72, 66, 56, 12, 2,
            M5.Display.color565(20, 14, 2));
        M5.Display.drawRoundRect(72, 66, 56, 12, 2, COLOR_AMBER);
        M5.Display.setTextColor(COLOR_AMBER,
            M5.Display.color565(20, 14, 2));
        M5.Display.setCursor(75, 69);
        M5.Display.print("LOOP ON");
    } else if (isShuffleEnabled) {
        M5.Display.fillRoundRect(72, 66, 56, 12, 2,
            M5.Display.color565(12, 8, 1));
        M5.Display.drawRoundRect(72, 66, 56, 12, 2, COLOR_WARM);
        M5.Display.setTextColor(COLOR_WARM,
            M5.Display.color565(12, 8, 1));
        M5.Display.setCursor(75, 69);
        M5.Display.print("SHUFFLE");
    } else {
        M5.Display.setTextColor(0x4208, COLOR_BG);
        M5.Display.setCursor(75, 69);
        M5.Display.print("SEQ");
    }
    
    M5.Display.drawFastHLine(0, 78, LEFT_W, 0x2945);
    
    // VU METER
    M5.Display.fillRect(0, 79, LEFT_W, 21, COLOR_BG);
    drawVuMeter();
    M5.Display.drawFastHLine(0, 100, LEFT_W, 0x2945);
    
    // KEYS
    M5.Display.fillRect(0, 101, LEFT_W, 17, COLOR_BG);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_BG);
    M5.Display.setCursor(2, 103);
    M5.Display.print("SPC");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Ply ");
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.print("+/-");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Vol ");
    M5.Display.setTextColor(COLOR_AMBER, COLOR_BG);
    M5.Display.print("`");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Exit");
    
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_BG);
    M5.Display.setCursor(2, 112);
    M5.Display.print(",");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Prv ");
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.print("/");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Nxt ");
    M5.Display.setTextColor(COLOR_AMBER, COLOR_BG);
    M5.Display.print("I");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Info");
}

// ========== ALT BAR ==========

void UnitAudioPlayerController::drawBottomBar() {
    M5.Display.fillRect(0, 118, 240, 17, COLOR_CREAM5);
    drawCreamLine(118, 0.6f);
    
    int vp = (currentVolume * 100) / 30;
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_CREAM5);
    M5.Display.setCursor(3, 123);
    M5.Display.print("VOL");
    
    int bX = 26, bY = 121, sW = 4, sH = 9, sG = 1, totB = 18;
    int act = (vp * totB) / 100;
    for (int i = 0; i < totB; i++) {
        if (i < act) {
            uint16_t c;
            if (i < 8) c = COLOR_CREAM2;
            else if (i < 13) c = COLOR_AMBER;
            else if (i < 16) c = COLOR_ORANGE;
            else c = COLOR_RED;
            M5.Display.fillRect(bX + i * (sW + sG), bY, sW, sH, c);
            M5.Display.drawFastHLine(bX + i * (sW + sG), bY, sW, COLOR_WHITE);
        } else {
            M5.Display.fillRect(bX + i * (sW + sG), bY, sW, sH,
                M5.Display.color565(6, 4, 0));
        }
    }
    
    char vstr[6]; 
    sprintf(vstr, "%3d%%", vp);
    M5.Display.setTextColor(COLOR_WHITE, COLOR_CREAM5);
    M5.Display.setCursor(122, 123);
    M5.Display.print(vstr);
    
    M5.Display.drawFastVLine(148, 119, 16, 0x4208);
    
    M5.Display.fillRect(149, 119, 91, 16, COLOR_CREAM5);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_CREAM5);
    M5.Display.setCursor(153, 123);
    char trk[16];
    sprintf(trk, "TRACK #%03d", currentTrack);
    M5.Display.print(trk);
}

// ========== ÜST BAR ==========

void UnitAudioPlayerController::drawTopBar() {
    for (int y = 0; y < 18; y++) {
        float t = (float)y / 17.0f;
        uint8_t rv = (uint8_t)(24 * (1.0f - t * 0.5f));
        uint8_t gv = (uint8_t)(18 * (1.0f - t * 0.5f));
        uint8_t bv = (uint8_t)(2 * (1.0f - t * 0.5f));
        M5.Display.drawFastHLine(0, y, 240,
            M5.Display.color565(rv, gv, bv));
    }
    drawCreamLine(17, 0.7f);
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1, 0x0000);
    M5.Display.setCursor(4, 5);
    M5.Display.print("M5");
    M5.Display.setTextColor(COLOR_AMBER, 0x0000);
    M5.Display.print(" AUDIO PLAYER");
    M5.Display.setTextColor(0x4208, 0x0000);
    M5.Display.print(" | Krem Edition");
}

// ========== ANA EKRAN ==========

void UnitAudioPlayerController::drawMainScreen() {
    M5.Display.fillScreen(COLOR_BG);
    drawTopBar();
    drawLeftPanel();
    
    for (int y = 18; y < 118; y++) {
        float t = (float)(y - 18) / 99.0f;
        uint8_t rv = (uint8_t)(8 + t * 10);
        uint8_t gv = (uint8_t)(6 + t * 8);
        M5.Display.drawPixel(LEFT_W, y,
            M5.Display.color565(rv, gv, 0));
    }
    drawFullDisk(diskAngle);
    drawBottomBar();
}

// ========== MESAJLAR ==========

void UnitAudioPlayerController::drawTempMsg() {
    int mw = LEFT_W - 4;
    M5.Display.fillRoundRect(2, 102, mw, 14, 3,
        M5.Display.color565(15, 10, 1));
    M5.Display.drawRoundRect(2, 102, mw, 14, 3, tempMessageColor);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_WHITE,
        M5.Display.color565(15, 10, 1));
    int tx = (mw - (int)tempMessageText.length() * 6) / 2 + 2;
    if (tx < 5) tx = 5;
    M5.Display.setCursor(tx, 105);
    M5.Display.print(tempMessageText);
}

void UnitAudioPlayerController::clearTempArea() {
    // Hot keys'i yeniden çiz
    M5.Display.fillRect(0, 101, LEFT_W, 17, COLOR_BG);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_BG);
    M5.Display.setCursor(2, 103);
    M5.Display.print("SPC");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Ply ");
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.print("+/-");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Vol ");
    M5.Display.setTextColor(COLOR_AMBER, COLOR_BG);
    M5.Display.print("`");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Exit");
    
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_BG);
    M5.Display.setCursor(2, 112);
    M5.Display.print(",");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Prv ");
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.print("/");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Nxt ");
    M5.Display.setTextColor(COLOR_AMBER, COLOR_BG);
    M5.Display.print("I");
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.print(":Info");
}

void UnitAudioPlayerController::showTempMsg(String msg, uint16_t col) {
    tempMessageText = msg;
    tempMessageColor = col;
    tempMessageTime = millis();
    tempMessageActive = true;
    drawTempMsg();
}

// ========== GÜNCELLEME ==========

void UnitAudioPlayerController::updateTrackDisplay() {
    M5.Display.fillRect(0, 36, LEFT_W, 28, COLOR_BG);
    char ts[6]; 
    sprintf(ts, "%03d", currentTrack);
    uint16_t numCol = isPlaying ? COLOR_CREAM1 : COLOR_CREAM2;
    M5.Display.setTextSize(3);
    if (isPlaying) {
        M5.Display.setTextColor(M5.Display.color565(12, 8, 0), COLOR_BG);
        M5.Display.setCursor(5, 39);
        M5.Display.print(ts);
        M5.Display.setTextColor(M5.Display.color565(20, 15, 4), COLOR_BG);
        M5.Display.setCursor(4, 38);
        M5.Display.print(ts);
    }
    M5.Display.setTextColor(numCol, COLOR_BG);
    M5.Display.setCursor(4, 37);
    M5.Display.print(ts);
    
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.setCursor(76, 37);
    M5.Display.print("TIME");
    
    M5.Display.fillRect(149, 119, 91, 16, COLOR_CREAM5);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_CREAM5);
    M5.Display.setCursor(153, 123);
    char trk[16]; 
    sprintf(trk, "TRACK #%03d", currentTrack);
    M5.Display.print(trk);
    
    M5.Display.fillRect(78, 19, LEFT_W - 78, 15, COLOR_CREAM5);
    char tot[12]; 
    sprintf(tot, "/%d", totalTracks);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_CREAM5);
    M5.Display.setCursor(82, 23);
    M5.Display.print(tot);
}

void UnitAudioPlayerController::updateStatusDisplay() {
    M5.Display.fillRect(0, 65, 72, 14, COLOR_BG);
    if (isPlaying) {
        M5.Display.fillRoundRect(2, 66, 66, 12, 2,
            M5.Display.color565(20, 14, 2));
        M5.Display.drawRoundRect(2, 66, 66, 12, 2, COLOR_CREAM1);
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(COLOR_CREAM1,
            M5.Display.color565(20, 14, 2));
        M5.Display.setCursor(6, 69);
        M5.Display.print(">> PLAYING");
    } else {
        M5.Display.fillRoundRect(2, 66, 66, 12, 2,
            M5.Display.color565(10, 8, 1));
        M5.Display.drawRoundRect(2, 66, 66, 12, 2, COLOR_CREAM3);
        M5.Display.setTextSize(1);
        M5.Display.setTextColor(COLOR_CREAM2,
            M5.Display.color565(10, 8, 1));
        M5.Display.setCursor(6, 69);
        M5.Display.print("|| PAUSED");
    }
}

void UnitAudioPlayerController::updateModeDisplay() {
    M5.Display.fillRect(72, 65, LEFT_W - 72, 14, COLOR_BG);
    M5.Display.setTextSize(1);
    if (isLoopEnabled) {
        M5.Display.fillRoundRect(72, 66, 56, 12, 2,
            M5.Display.color565(20, 14, 2));
        M5.Display.drawRoundRect(72, 66, 56, 12, 2, COLOR_AMBER);
        M5.Display.setTextColor(COLOR_AMBER,
            M5.Display.color565(20, 14, 2));
        M5.Display.setCursor(75, 69);
        M5.Display.print("LOOP ON");
    } else if (isShuffleEnabled) {
        M5.Display.fillRoundRect(72, 66, 56, 12, 2,
            M5.Display.color565(12, 8, 1));
        M5.Display.drawRoundRect(72, 66, 56, 12, 2, COLOR_WARM);
        M5.Display.setTextColor(COLOR_WARM,
            M5.Display.color565(12, 8, 1));
        M5.Display.setCursor(75, 69);
        M5.Display.print("SHUFFLE");
    } else {
        M5.Display.setTextColor(0x4208, COLOR_BG);
        M5.Display.setCursor(75, 69);
        M5.Display.print("SEQ");
    }
}

void UnitAudioPlayerController::updateTimeDisplay() {
    unsigned long el = isPlaying ?
        (millis() - trackStartTime - totalPausedTime) / 1000 : 0;
    M5.Display.fillRect(72, 46, 58, 12, COLOR_BG);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_WHITE, COLOR_BG);
    M5.Display.setCursor(74, 47);
    M5.Display.printf("%02d:%02d", (int)(el / 60), (int)(el % 60));
}

void UnitAudioPlayerController::updateMainDisplay() {
    unsigned long now = millis();
    
    if (now - lastDiskUpdate >= diskUpdateInterval) {
        lastDiskUpdate = now;
        updateDiskRotation();
        bool pChg = (lastDiskPlaying != isPlaying);
        if (diskSpeed > 0.01f || pChg) {
            if (pChg) { 
                drawFullDisk(diskAngle); 
                lastDiskPlaying = isPlaying; 
            } else {
                drawDiskDynamic(diskAngle);
            }
        }
    }
    
    if (now - lastVuUpdate >= vuUpdateInterval) {
        lastVuUpdate = now;
        updateVuMeter();
        drawVuMeter();
    }
    
    if (now - lastTimeUpdate >= timeUpdateInterval) {
        lastTimeUpdate = now;
        updateTimeDisplay();
    }
    
    if (tempMessageActive && (now - tempMessageTime > tempMessageDuration)) {
        tempMessageActive = false;
        clearTempArea();
    }
}

// ========== INFO EKRANI ==========

void UnitAudioPlayerController::enterInfoScreen() {
    isInfoScreen = true;
    M5.Display.fillScreen(COLOR_BG);
    drawCreamLine(0, 0.9f);
    
    M5.Display.fillRect(0, 1, 240, 14, COLOR_CREAM5);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_CREAM5);
    M5.Display.setCursor(48, 4);
    M5.Display.print("M5 AUDIO PLAYER - KEYS");
    drawCreamLine(15, 0.5f);
    
    struct KeyInfo { const char* key; const char* desc; };
    KeyInfo keys[] = {
        {"SPACE", "Play/Pause"},
        {"+", "Vol Up"},
        {"-", "Vol Down"},
        {"R", "Random"},
        {"L", "Loop On/Off"},
        {"S", "Shuffle"},
        {",", "Prev Track"},
        {"/", "Next Track"},
        {".", "-10 Tracks"},
        {";", "+10 Tracks"},
        {"I", "Info (Toggle)"},
        {"`", "Exit to Menu"},
    };
    
    int total = sizeof(keys) / sizeof(keys[0]);
    int half = (total + 1) / 2;
    int ky = 19;
    
    for (int i = 0; i < half; i++) {
        M5.Display.fillRoundRect(2, ky, 36, 9, 1, COLOR_CREAM5);
        M5.Display.drawRoundRect(2, ky, 36, 9, 1, COLOR_CREAM2);
        M5.Display.setTextColor(COLOR_CREAM1, COLOR_CREAM5);
        M5.Display.setCursor(4, ky + 1);
        M5.Display.print(keys[i].key);
        M5.Display.setTextColor(0xDECB, COLOR_BG);
        M5.Display.setCursor(41, ky + 1);
        M5.Display.print(keys[i].desc);
        
        int j = i + half;
        if (j < total) {
            M5.Display.fillRoundRect(122, ky, 36, 9, 1, COLOR_CREAM5);
            M5.Display.drawRoundRect(122, ky, 36, 9, 1, COLOR_AMBER);
            M5.Display.setTextColor(COLOR_AMBER, COLOR_CREAM5);
            M5.Display.setCursor(124, ky + 1);
            M5.Display.print(keys[j].key);
            M5.Display.setTextColor(0xDECB, COLOR_BG);
            M5.Display.setCursor(161, ky + 1);
            M5.Display.print(keys[j].desc);
        }
        ky += 11;
    }
    
    drawCreamLine(126, 0.4f);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(0x6B4D, COLOR_BG);
    M5.Display.setCursor(52, 129);
    M5.Display.print("Press I or ` to exit");
}

void UnitAudioPlayerController::exitInfoScreen() {
    isInfoScreen = false;
    lastDiskPlaying = !isPlaying;
    drawMainScreen();
}

// ========== KONTROL ==========

void UnitAudioPlayerController::volumeUp() {
    if (currentVolume < 30) {
        currentVolume++;
        audioplayer.setVolume(currentVolume);
        char m[20]; 
        sprintf(m, "VOL %d%%", (currentVolume * 100) / 30);
        drawBottomBar();
        showTempMsg(m, COLOR_CREAM1);
    }
}

void UnitAudioPlayerController::volumeDown() {
    if (currentVolume > 0) {
        currentVolume--;
        audioplayer.setVolume(currentVolume);
        char m[20]; 
        sprintf(m, "VOL %d%%", (currentVolume * 100) / 30);
        drawBottomBar();
        showTempMsg(m, COLOR_CREAM2);
    }
}

void UnitAudioPlayerController::toggleLoop() {
    isLoopEnabled = !isLoopEnabled;
    if (isLoopEnabled && isShuffleEnabled) isShuffleEnabled = false;
    String msg = isLoopEnabled ? "LOOP ON" : "LOOP OFF";
    updateModeDisplay();
    showTempMsg(msg, COLOR_AMBER);
}

void UnitAudioPlayerController::toggleShuffle() {
    isShuffleEnabled = !isShuffleEnabled;
    if (isShuffleEnabled && isLoopEnabled) isLoopEnabled = false;
    String msg = isShuffleEnabled ? "SHUFFLE ON" : "SHUFFLE OFF";
    updateModeDisplay();
    showTempMsg(msg, COLOR_WARM);
}

void UnitAudioPlayerController::randomTrack() {
    if (totalTracks == 0) return;
    uint16_t rnd = random(1, totalTracks + 1);
    audioplayer.selectAudioNum(rnd);
    currentTrack = rnd; 
    lastDisplayedTrack = rnd;
    trackStartTime = millis(); 
    totalPausedTime = 0;
    if (lastPlayStatus == AUDIO_PLAYER_STATUS_PLAYING) {
        audioplayer.playAudio(); 
        isPlaying = true;
    }
    char m[15]; 
    sprintf(m, "RND %d!", rnd);
    updateTrackDisplay();
    showTempMsg(m, COLOR_AMBER);
}

void UnitAudioPlayerController::goToTrack(int n) {
    if (n < 1 || n > (int)totalTracks) return;
    audioplayer.selectAudioNum(n); 
    delay(100);
    audioplayer.playAudio();
    isPlaying = true; 
    trackStartTime = millis(); 
    totalPausedTime = 0;
    currentTrack = n; 
    lastDisplayedTrack = n;
    char m[15]; 
    sprintf(m, "GOTO %d", n);
    updateTrackDisplay(); 
    updateStatusDisplay();
    showTempMsg(m, COLOR_CREAM1);
}

void UnitAudioPlayerController::executeAction(int clicks) {
    if (clicks == 1) {
        uint8_t st = audioplayer.checkPlayStatus();
        if (st == AUDIO_PLAYER_STATUS_PLAYING) {
            audioplayer.pauseAudio(); 
            pausedTime = millis(); 
            isPlaying = false;
            updateStatusDisplay(); 
            showTempMsg("PAUSED", COLOR_CREAM2);
        } else {
            audioplayer.playAudio();
            if (st == AUDIO_PLAYER_STATUS_STOPPED) { 
                trackStartTime = millis(); 
                totalPausedTime = 0; 
            } else {
                totalPausedTime += (millis() - pausedTime);
            }
            isPlaying = true;
            updateStatusDisplay(); 
            showTempMsg("PLAYING", COLOR_CREAM1);
        }
    } else if (clicks == 2) {
        audioplayer.nextAudio(); 
        delay(100); 
        audioplayer.playAudio();
        isPlaying = true; 
        trackStartTime = millis(); 
        totalPausedTime = 0;
        updateStatusDisplay(); 
        showTempMsg("NEXT >>", COLOR_CREAM1);
    } else if (clicks == 3) {
        audioplayer.previousAudio(); 
        delay(100); 
        audioplayer.playAudio();
        isPlaying = true; 
        trackStartTime = millis(); 
        totalPausedTime = 0;
        updateStatusDisplay(); 
        showTempMsg("<< PREV", COLOR_CREAM1);
    }
}

void UnitAudioPlayerController::checkAutoNext() {
    static unsigned long lc = 0;
    if (millis() - lc < 500) return;
    lc = millis();
    uint8_t cs = audioplayer.checkPlayStatus();
    
    if (lastPlayStatus == AUDIO_PLAYER_STATUS_PLAYING &&
        cs == AUDIO_PLAYER_STATUS_STOPPED) {
        if (isLoopEnabled) {
            delay(100); 
            audioplayer.playAudio(); 
            delay(100);
            trackStartTime = millis(); 
            totalPausedTime = 0; 
            isPlaying = true;
            if (!isInfoScreen) showTempMsg("LOOP", COLOR_AMBER);
        } else if (isShuffleEnabled) {
            uint16_t r = random(1, totalTracks + 1);
            audioplayer.selectAudioNum(r); 
            delay(200);
            audioplayer.playAudio(); 
            delay(100);
            trackStartTime = millis(); 
            totalPausedTime = 0; 
            isPlaying = true;
            if (!isInfoScreen) showTempMsg("SHUFFLE", COLOR_WARM);
        } else {
            audioplayer.nextAudio(); 
            delay(200);
            audioplayer.playAudio(); 
            delay(100);
            trackStartTime = millis(); 
            totalPausedTime = 0; 
            isPlaying = true;
            if (!isInfoScreen) showTempMsg("AUTO NEXT", COLOR_CREAM1);
        }
    }
    
    uint16_t rt = audioplayer.getCurrentAudioNumber();
    if (rt < 1 || rt > totalTracks) return;
    
    if (rt != lastDisplayedTrack) {
        currentTrack = rt; 
        lastDisplayedTrack = rt;
        trackStartTime = millis(); 
        totalPausedTime = 0;
        if (!isInfoScreen) updateTrackDisplay();
    }
    
    if (lastPlayStatus != cs) {
        if (cs == AUDIO_PLAYER_STATUS_PLAYING) {
            if (lastPlayStatus == AUDIO_PLAYER_STATUS_PAUSED)
                totalPausedTime += (millis() - pausedTime);
            isPlaying = true;
        } else if (cs == AUDIO_PLAYER_STATUS_PAUSED) {
            pausedTime = millis(); 
            isPlaying = false;
        } else {
            isPlaying = false;
        }
        if (!isInfoScreen) updateStatusDisplay();
    }
    lastPlayStatus = cs;
}

// ========== ANA FONKSİYONLAR ==========

void UnitAudioPlayerController::Begin() {
    showTopBar = false;
    M5.Display.fillScreen(COLOR_BG);
    
    // Bağlantı ekranı
    drawCreamLine(0, 0.9f);
    drawCreamLine(134, 0.9f);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_CREAM1, COLOR_BG);
    M5.Display.setCursor(30, 40);
    M5.Display.print("CONNECTING");
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM2, COLOR_BG);
    M5.Display.setCursor(42, 62);
    M5.Display.print("Audio Player Module");
    M5.Display.setTextColor(0x4208, COLOR_BG);
    M5.Display.setCursor(50, 74);
    M5.Display.print("Port A: G1 / G2");
    
    int8_t port_a_pin1 = M5.getPin(m5::pin_name_t::port_a_pin1);
    int8_t port_a_pin2 = M5.getPin(m5::pin_name_t::port_a_pin2);
    
    int dotX = 60;
    int counter = 0;
    while (!audioplayer.begin(&Serial1, port_a_pin1, port_a_pin2)) {
        if (counter > 5) {
            mainOS->ChangeMenu(new Extra(mainOS));
            return;
        }
        for (int i = 0; i < 5; i++) {
            uint8_t rv = (uint8_t)(10 + i * 4);
            uint8_t gv = (uint8_t)(8 + i * 3);
            M5.Display.fillRect(dotX + i * 8, 90, 6, 8,
                M5.Display.color565(rv, gv, 0));
        }
        dotX += 8;
        if (dotX > 180) {
            M5.Display.fillRect(55, 90, 180, 10, COLOR_BG);
            dotX = 60;
        }
        delay(400);
        counter++;
    }
    
    M5.Display.fillRect(30, 86, 180, 18, COLOR_BG);
    M5.Display.fillRoundRect(40, 88, 160, 14, 3,
        M5.Display.color565(15, 10, 1));
    M5.Display.drawRoundRect(40, 88, 160, 14, 3, COLOR_CREAM1);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(COLOR_CREAM1,
        M5.Display.color565(15, 10, 1));
    M5.Display.setCursor(80, 92);
    M5.Display.print("CONNECTED!");
    delay(600);
    
    audioplayer.setVolume(currentVolume);
    audioplayer.setPlayMode(AUDIO_PLAYER_MODE_SINGLE_STOP);
    delay(300);
    totalTracks = audioplayer.getTotalAudioNumber();
    audioplayer.selectAudioNum(1);
    currentTrack = 1; 
    lastDisplayedTrack = 1;
    lastDiskPlaying = !isPlaying;
    
    randomSeed(analogRead(0));
    drawMainScreen();
}

void UnitAudioPlayerController::Loop() {
    if (isInfoScreen) {
        if (mainOS->NewKey.ifKeyJustPress('i') || 
            mainOS->NewKey.ifKeyJustPress('I') ||
            mainOS->NewKey.ifKeyJustPress('`')) {
            exitInfoScreen();
            return;
        }
        checkAutoNext();
        return;
    }
    
    updateMainDisplay();
    
    // Exit
    if (mainOS->NewKey.ifKeyJustPress('`')) {
        mainOS->ChangeMenu(new Extra(mainOS));
        return;
    }
    
    // Volume
    if (mainOS->NewKey.ifKeyJustPress('+') || 
        mainOS->NewKey.ifKeyJustPress('=')) {
        volumeUp();
    }
    if (mainOS->NewKey.ifKeyJustPress('-')) {
        volumeDown();
    }
    
    // Track navigation
    if (mainOS->NewKey.ifKeyJustPress(',')) {
        executeAction(3); // Previous
    }
    if (mainOS->NewKey.ifKeyJustPress('/')) {
        executeAction(2); // Next
    }
    if (mainOS->NewKey.ifKeyJustPress('.')) {
        goToTrack(max((int)currentTrack - 10, 1));
    }
    if (mainOS->NewKey.ifKeyJustPress(';')) {
        goToTrack(min((int)currentTrack + 10, (int)totalTracks));
    }
    
    // Modes
    if (mainOS->NewKey.ifKeyJustPress('r') || 
        mainOS->NewKey.ifKeyJustPress('R')) {
        randomTrack();
    }
    if (mainOS->NewKey.ifKeyJustPress('l') || 
        mainOS->NewKey.ifKeyJustPress('L')) {
        toggleLoop();
    }
    if (mainOS->NewKey.ifKeyJustPress('s') || 
        mainOS->NewKey.ifKeyJustPress('S')) {
        toggleShuffle();
    }
    
    // Info
    if (mainOS->NewKey.ifKeyJustPress('i') || 
        mainOS->NewKey.ifKeyJustPress('I')) {
        enterInfoScreen();
    }
    
    // Play/Pause
    if (mainOS->NewKey.ifKeyJustPress(' ')) {
        executeAction(1);
    }
    
    checkAutoNext();
}

void UnitAudioPlayerController::Draw() {
    // Loop içinde çizim yapıldığı için boş
}
