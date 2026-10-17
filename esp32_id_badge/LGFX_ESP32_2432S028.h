#pragma once

// ============================================================================
// LovyanGFX Hardware Configuration for ESP32-2432S028 (CYD / Cheap Yellow Display)
// ============================================================================
// Display: 2.8" TFT 240x320 (ILI9341 or ST7789) on HSPI (GPIO 14, 13, 12, 15, 2)
// Touch:   XPT2046 Resistive Touch via Software SPI (GPIO 25, 32, 39, 33)
//          *Using Software SPI prevents SPI bus collisions with the MicroSD card!
// RGB LED: Pins 4 (R), 16 (G), 17 (B) - Active LOW
// LDR:     Pin 34 (Analog Light Sensor)
//
// Driver Selection:
//   0 = AUTO_DETECT (LovyanGFX probes display ID: automatically picks ILI9341 or ST7789)
//   1 = FORCE ILI9341 (For original single-USB CYD boards)
//   2 = FORCE ST7789  (For newer dual-USB Type-C + Micro-USB CYD boards)
// ============================================================================
#ifndef CYD_DISPLAY_MODE
#define CYD_DISPLAY_MODE 0
#endif

#if CYD_DISPLAY_MODE == 0

// Use LovyanGFX built-in autodetector for Sunton ESP32-2432S028
#define LGFX_SUNTON_ESP32_2432S028
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#elif CYD_DISPLAY_MODE == 1

// Manual ILI9341 Configuration (240x320 Native Portrait)
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void) {
    // 1. HSPI Bus for Display
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host   = HSPI_HOST; // SPI2
      cfg.spi_mode   = 0;
      cfg.freq_write = 40000000;  // 40MHz
      cfg.freq_read  = 16000000;
      cfg.pin_sclk   = 14;        // TFT SCK
      cfg.pin_mosi   = 13;        // TFT MOSI
      cfg.pin_miso   = 12;        // TFT MISO
      cfg.pin_dc     = 2;         // TFT DC / RS
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    // 2. ILI9341 Display Panel (240x320 Native Portrait)
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 15;   // TFT CS
      cfg.pin_rst          = -1;   // Connected to EN
      cfg.panel_width      = 240;  // True native panel width
      cfg.panel_height     = 320;  // True native panel height
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 2;    // Rotation offset 2 matches CYD board orientation
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable         = true;
      cfg.invert           = false;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = false;
      _panel_instance.config(cfg);
    }

    // 3. Backlight Control (PWM on GPIO 21)
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl      = 21;        // Backlight pin
      cfg.invert      = false;     // Active HIGH
      cfg.freq        = 44100;     // PWM frequency
      cfg.pwm_channel = 7;         // PWM channel
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    // 4. Touch Screen Controller (XPT2046 with Software SPI)
    {
      auto cfg = _touch_instance.config();
      cfg.pin_int    = -1;        // Touch IRQ (-1 for reliable polling on CYD)
      cfg.spi_host   = -1;        // -1: Use Software SPI (avoids SPI conflict with MicroSD card)
      cfg.freq       = 2500000;   // 2.5MHz
      cfg.pin_sclk   = 25;        // Touch CLK
      cfg.pin_mosi   = 32;        // Touch DIN / MOSI
      cfg.pin_miso   = 39;        // Touch DO / MISO
      cfg.pin_cs     = 33;        // Touch CS
      cfg.bus_shared = false;
      cfg.x_min      = 300;
      cfg.x_max      = 3900;
      cfg.y_min      = 3700;
      cfg.y_max      = 200;
      cfg.offset_rotation = 2;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

#elif CYD_DISPLAY_MODE == 2

// Manual ST7789 Configuration (240x320 Native Portrait)
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789  _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void) {
    // 1. HSPI Bus for Display
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host   = HSPI_HOST; // SPI2
      cfg.spi_mode   = 0;
      cfg.freq_write = 40000000;  // 40MHz
      cfg.freq_read  = 16000000;
      cfg.pin_sclk   = 14;        // TFT SCK
      cfg.pin_mosi   = 13;        // TFT MOSI
      cfg.pin_miso   = 12;        // TFT MISO
      cfg.pin_dc     = 2;         // TFT DC / RS
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    // 2. ST7789 Display Panel (240x320 Native Portrait)
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 15;   // TFT CS
      cfg.pin_rst          = -1;   // Connected to EN
      cfg.panel_width      = 240;  // True native panel width
      cfg.panel_height     = 320;  // True native panel height
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 2;    // Rotation offset 2 matches CYD board orientation
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable         = true;
      cfg.invert           = true; // ST7789 on CYD requires invert = true
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = false;
      _panel_instance.config(cfg);
    }

    // 3. Backlight Control (PWM on GPIO 21)
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl      = 21;        // Backlight pin
      cfg.invert      = false;     // Active HIGH
      cfg.freq        = 44100;     // PWM frequency
      cfg.pwm_channel = 7;         // PWM channel
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    // 4. Touch Screen Controller (XPT2046 with Software SPI)
    {
      auto cfg = _touch_instance.config();
      cfg.pin_int    = -1;        // Touch IRQ (-1 for reliable polling on CYD)
      cfg.spi_host   = -1;        // -1: Use Software SPI (avoids SPI conflict with MicroSD card)
      cfg.freq       = 2500000;   // 2.5MHz
      cfg.pin_sclk   = 25;        // Touch CLK
      cfg.pin_mosi   = 32;        // Touch DIN / MOSI
      cfg.pin_miso   = 39;        // Touch DO / MISO
      cfg.pin_cs     = 33;        // Touch CS
      cfg.bus_shared = false;
      cfg.x_min      = 300;
      cfg.x_max      = 3900;
      cfg.y_min      = 3700;
      cfg.y_max      = 200;
      cfg.offset_rotation = 2;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

#endif

// Onboard Hardware Pins
#define CYD_LED_RED     4    // Active LOW (LOW = ON)
#define CYD_LED_GREEN   16   // Active LOW
#define CYD_LED_BLUE    17   // Active LOW
#define CYD_LDR_PIN     34   // Ambient Light Sensor

