/*
 * ============================================================================
 * DIGITAL ID BADGE / SMART ACCESS CARD FOR ESP32-2432S028 (CYD)
 * PlatformIO Source File (src/main.cpp)
 * ============================================================================
 * Hardware:
 *   - Board: ESP32-2432S028 ("Cheap Yellow Display" / CYD)
 *   - Screen: 2.8-inch TFT LCD (240x320 Portrait)
 *   - Touch: XPT2046 Resistive Touch
 *   - RGB LED: GPIO 4 (Red), GPIO 16 (Green), GPIO 17 (Blue) - Active LOW
 *
 * Features:
 *   - High-tech Cyber / Glassmorphic ID Badge UI
 *   - Embedded photo rendered from flash memory (JPEG decoded via hardware)
 *   - Interactive Touch Screen: Tap anywhere to flip between FRONT and BACK of badge!
 *   - Back side features a live scannable QR code generated in real-time
 *   - Onboard RGB LED status indicators (Emerald Green = Active, Cyan = Credentials, Flash on Tap)
 * ============================================================================
 */

#include <Arduino.h>
#include "LGFX_ESP32_2432S028.h"
#include "photo_data.h"

// Instantiate the display controller
LGFX lcd;

// ============================================================================
// 1. USER PROFILE CUSTOMIZATION
// (Change these values to whatever you want displayed on your ID badge!)
// ============================================================================
const char* ID_NAME        = "SAMIR SHRESTHA";             // Your Full Name
const char* ID_POST        = "EMBEDDED DEVELOPER";         // Your Job Post / Role
const char* ID_NUMBER      = "ID: ESP-2432";               // ID / Employee Number
const char* ID_DEPARTMENT  = "IOT & EMBEDDED SYSTEMS";     // Department / Organization
const char* ID_STATUS      = "ACTIVE";                     // Card Status (ACTIVE / VERIFIED)
const char* ID_BLOOD_GROUP = "O+";                         // Blood Group
const char* ID_EXPIRY      = "12 / 2028";                  // Validity / Expiry Date
const char* ID_CONTACT     = "+977 980-000-0000";          // Emergency / Contact Phone
const char* QR_LINK        = "https://github.com";         // Live QR Code Target URL / Text

// Screen Orientation (Portrait for ID Badge):
// Set to 1 (standard Portrait: USB port at bottom) or 3 (inverted Portrait: USB port at top)
#define SCREEN_ROTATION    1

// ============================================================================
// 2. COLOR PALETTE (Modern Slate & Cyan Theme)
// ============================================================================
#define COLOR_BG           lcd.color565(15, 23, 42)    // Slate 900 Background
#define COLOR_CARD         lcd.color565(24, 32, 54)    // Slate 800 Card Surface
#define COLOR_BORDER       lcd.color565(56, 189, 248)  // Vibrant Sky Cyan
#define COLOR_HEADER_BG    lcd.color565(30, 41, 59)    // Dark Navy
#define COLOR_PILL         lcd.color565(14, 116, 144)  // Cyan 700 Accent
#define COLOR_PILL_TEXT    lcd.color565(240, 253, 250) // Mint Crisp White
#define COLOR_TEXT_MAIN    lcd.color565(255, 255, 255) // Pure White
#define COLOR_TEXT_MUTED   lcd.color565(148, 163, 184) // Slate 400 Muted Text
#define COLOR_STATUS_GREEN lcd.color565(52, 211, 153)  // Emerald Active Green
#define COLOR_DIVIDER      lcd.color565(51, 65, 85)    // Slate 700 Border Line
#define COLOR_ROW_ALT      lcd.color565(33, 43, 67)    // Alternating Row Dark Tint

// State Management
bool isFrontView = true;
uint32_t lastTouchTime = 0;

// ============================================================================
// 3. RGB LED CONTROL (Active LOW)
// ============================================================================
void setRgbLed(bool red, bool green, bool blue) {
  digitalWrite(CYD_LED_RED,   red   ? LOW : HIGH);
  digitalWrite(CYD_LED_GREEN, green ? LOW : HIGH);
  digitalWrite(CYD_LED_BLUE,  blue  ? LOW : HIGH);
}

void flashLedPulse() {
  // Quick white flash for touch feedback
  setRgbLed(true, true, true);
  delay(50);
  if (isFrontView) {
    setRgbLed(false, true, false); // Emerald Green for Front Active
  } else {
    setRgbLed(false, true, true);  // Cyan for Back Credentials
  }
}

// ============================================================================
// 4. DRAW FRONT CARD (Photo, Name, Post, Barcode)
// ============================================================================
void drawFrontCard() {
  lcd.startWrite();
  lcd.fillScreen(COLOR_BG);

  // Outer Rounded Card Shell
  lcd.fillRoundRect(4, 4, 232, 312, 12, COLOR_CARD);
  lcd.drawRoundRect(4, 4, 232, 312, 12, COLOR_BORDER);
  lcd.drawRoundRect(5, 5, 230, 310, 11, COLOR_BORDER);

  // Simulated Lanyard Hole / Slot at top
  lcd.fillRoundRect(98, 8, 44, 7, 3, COLOR_BG);
  lcd.drawRoundRect(98, 8, 44, 7, 3, COLOR_TEXT_MUTED);

  // Top Header Banner
  lcd.fillRoundRect(12, 18, 216, 30, 6, COLOR_HEADER_BG);
  lcd.drawRoundRect(12, 18, 216, 30, 6, COLOR_DIVIDER);

  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.setTextColor(COLOR_BORDER);
  lcd.setFont(&fonts::Font0);
  lcd.setTextSize(1);
  lcd.drawString("DIGITAL ID BADGE", 120, 26);

  lcd.setTextColor(COLOR_TEXT_MUTED);
  lcd.drawString("SYSTEM ACCESS CARD", 120, 39);

  // Photo Frame (Photo size: 130 x 150)
  const int photo_x = 55;
  const int photo_y = 54;
  lcd.fillRoundRect(photo_x - 4, photo_y - 4, 138, 158, 6, COLOR_BORDER);
  lcd.drawRoundRect(photo_x - 1, photo_y - 1, 132, 152, 2, COLOR_TEXT_MAIN);

  // Render High-Quality JPEG Photo
  lcd.drawJpg(photo_jpg, sizeof(photo_jpg), photo_x, photo_y);

  // Name (Prominent Centered)
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.setTextColor(COLOR_TEXT_MAIN);
  lcd.setFont(&fonts::Font2);
  lcd.setTextSize(1);
  lcd.drawString(ID_NAME, 120, 222);

  // Post / Designation Pill Badge
  const int pill_y = 240;
  lcd.fillRoundRect(35, pill_y - 9, 170, 18, 9, COLOR_PILL);
  lcd.drawRoundRect(35, pill_y - 9, 170, 18, 9, COLOR_BORDER);

  lcd.setFont(&fonts::Font0);
  lcd.setTextSize(1);
  lcd.setTextColor(COLOR_PILL_TEXT);
  lcd.drawString(ID_POST, 120, pill_y);

  // Subtle Divider Line
  lcd.drawFastHLine(18, 256, 204, COLOR_DIVIDER);

  // ID Number (Left) and Status (Right)
  lcd.setTextDatum(textdatum_t::middle_left);
  lcd.setTextColor(COLOR_TEXT_MUTED);
  lcd.drawString(ID_NUMBER, 22, 266);

  lcd.setTextDatum(textdatum_t::middle_right);
  lcd.fillCircle(145, 266, 3, COLOR_STATUS_GREEN); // Glowing Active Dot
  lcd.setTextColor(COLOR_STATUS_GREEN);
  lcd.drawString(ID_STATUS, 218, 266);

  // Realistic Barcode Simulation
  const int bar_y = 277;
  lcd.fillRect(20, bar_y, 200, 24, TFT_WHITE);
  
  uint16_t seed = 0xACE1;
  int bx = 26;
  while (bx < 214) {
    seed = (seed >> 1) ^ (-(seed & 1u) & 0xB400u);
    int w = 1 + (seed % 3);
    lcd.fillRect(bx, bar_y + 2, w, 20, TFT_BLACK);
    seed = (seed >> 1) ^ (-(seed & 1u) & 0xB400u);
    bx += w + 1 + (seed % 3);
  }

  // Footer Serial Key / Touch Hint
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.setTextColor(COLOR_TEXT_MUTED);
  lcd.drawString("* 2432S028-SECURE-KEY *", 120, 308);

  lcd.endWrite();
  setRgbLed(false, true, false); // Green status LED
}

// ============================================================================
// 5. DRAW BACK CARD (Scannable QR Code & Verified Credentials)
// ============================================================================
void drawBackCard() {
  lcd.startWrite();
  lcd.fillScreen(COLOR_BG);

  // Outer Rounded Card Shell
  lcd.fillRoundRect(4, 4, 232, 312, 12, COLOR_CARD);
  lcd.drawRoundRect(4, 4, 232, 312, 12, COLOR_BORDER);
  lcd.drawRoundRect(5, 5, 230, 310, 11, COLOR_BORDER);

  // Simulated Lanyard Hole / Slot at top
  lcd.fillRoundRect(98, 8, 44, 7, 3, COLOR_BG);
  lcd.drawRoundRect(98, 8, 44, 7, 3, COLOR_TEXT_MUTED);

  // Top Header Banner
  lcd.fillRoundRect(12, 18, 216, 30, 6, COLOR_HEADER_BG);
  lcd.drawRoundRect(12, 18, 216, 30, 6, COLOR_DIVIDER);

  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.setTextColor(COLOR_BORDER);
  lcd.setFont(&fonts::Font0);
  lcd.setTextSize(1);
  lcd.drawString("SECURITY CREDENTIALS", 120, 26);

  lcd.setTextColor(COLOR_TEXT_MUTED);
  lcd.drawString(ID_DEPARTMENT, 120, 39);

  // QR Code Frame (100 x 100)
  const int q_x = 70;
  const int q_y = 54;
  lcd.fillRoundRect(q_x - 5, q_y - 5, 110, 110, 6, TFT_WHITE);
  lcd.drawRoundRect(q_x - 6, q_y - 6, 112, 112, 6, COLOR_BORDER);

  // Real-time generated scannable QR Code!
  lcd.qrcode(QR_LINK, q_x, q_y, 100, 2);

  // QR Subtitle
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.setTextColor(COLOR_BORDER);
  lcd.drawString("[ SCAN FOR VERIFICATION ]", 120, 172);

  // Credentials Information Table
  const int table_y = 184;
  const int row_h = 18;
  const char* labels[] = {"NAME:", "ROLE:", "DEPT:", "BLOOD:", "VALID:", "EMERGENCY:"};
  const char* values[] = {ID_NAME, ID_POST, "EMBEDDED / IOT", ID_BLOOD_GROUP, ID_EXPIRY, ID_CONTACT};

  lcd.setFont(&fonts::Font0);
  for (int i = 0; i < 6; i++) {
    int cy = table_y + (i * row_h);
    if (i % 2 == 0) {
      lcd.fillRoundRect(14, cy - 8, 212, row_h - 2, 3, COLOR_ROW_ALT);
    }
    lcd.setTextDatum(textdatum_t::middle_left);
    lcd.setTextColor(COLOR_TEXT_MUTED);
    lcd.drawString(labels[i], 18, cy);

    lcd.setTextDatum(textdatum_t::middle_right);
    lcd.setTextColor(COLOR_TEXT_MAIN);
    lcd.drawString(values[i], 222, cy);
  }

  // Footer Navigation Hint
  lcd.setTextDatum(textdatum_t::middle_center);
  lcd.setTextColor(COLOR_BORDER);
  lcd.drawString("<< TAP SCREEN TO FLIP >>", 120, 305);

  lcd.endWrite();
  setRgbLed(false, true, true); // Cyan status LED
}

// ============================================================================
// 6. SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-2432S028 Digital ID Badge starting...");

  // Initialize RGB LED Pins
  pinMode(CYD_LED_RED,   OUTPUT);
  pinMode(CYD_LED_GREEN, OUTPUT);
  pinMode(CYD_LED_BLUE,  OUTPUT);
  setRgbLed(true, true, true); // White LED on boot

  // Initialize LCD Display
  lcd.init();
  lcd.setRotation(SCREEN_ROTATION);
  lcd.setBrightness(255); // Full backlight brightness

  // Render initial Front ID Card
  drawFrontCard();
  Serial.println("ID Badge loaded successfully.");
}

// ============================================================================
// 7. MAIN LOOP (Touch detection & interactive flipping)
// ============================================================================
void loop() {
  int32_t touch_x, touch_y;

  // Check if screen is touched
  if (lcd.getTouch(&touch_x, &touch_y)) {
    // 400ms debounce to prevent double-triggering
    if (millis() - lastTouchTime > 400) {
      lastTouchTime = millis();
      
      // Toggle card face
      isFrontView = !isFrontView;
      flashLedPulse();

      if (isFrontView) {
        drawFrontCard();
      } else {
        drawBackCard();
      }
    }
  }

  delay(20);
}
