#pragma once

// ============================================================================
// LovyanGFX Hardware Configuration for ESP32-2432S028 (CYD / Cheap Yellow Display)
// Exact Geometry from Launcher: TFT_WIDTH=240, TFT_HEIGHT=320, ROTATION=0
// ============================================================================
// Display: 2.8" TFT 240x320 SPI (ILI9341) on HSPI (GPIO 14, 13, 12, 15, 2)
// Touch:   XPT2046 Resistive Touch via Software SPI (GPIO 25, 32, 39, 33)
// RGB LED: Pins 4 (R), 16 (G), 17 (B) - Active LOW
// LDR:     Pin 34 (Analog Light Sensor)
// ============================================================================

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Color inversion option (CYD-2-USB models sometimes require color inversion)
#ifndef CYD_INVERT_COLORS
#define CYD_INVERT_COLORS false
#endif

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void) {
    // 1. HSPI Bus for Display (GPIO 14 SCLK, 13 MOSI, 12 MISO, 2 DC, 15 CS)
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

    // 2. ILI9341 Display Panel (240x320 Native Portrait - matched with Launcher)
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 15;   // TFT CS
      cfg.pin_rst          = -1;   // Reset connected to EN
      cfg.panel_width      = 240;  // TFT_WIDTH = 240
      cfg.panel_height     = 320;  // TFT_HEIGHT = 320
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 0;    // ROT_OFFSET = 0 (Matches Launcher)
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable         = true;
      cfg.invert           = CYD_INVERT_COLORS;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = false;
      _panel_instance.config(cfg);
    }

    // 3. Backlight Control (PWM on GPIO 21)
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl      = 21;        // TFT_BL = 21
      cfg.invert      = false;     // Active HIGH
      cfg.freq        = 44100;     // PWM frequency
      cfg.pwm_channel = 7;         // PWM channel
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    // 4. Touch Screen Controller (XPT2046 with Software SPI)
    {
      auto cfg = _touch_instance.config();
      cfg.pin_int    = -1;        // Touch IRQ (GPIO 36)
      cfg.spi_host   = -1;        // Software SPI (prevents SPI conflict with MicroSD on VSPI)
      cfg.freq       = 2500000;   // 2.5MHz
      cfg.pin_sclk   = 25;        // Touch CLK
      cfg.pin_mosi   = 32;        // Touch DIN / MOSI
      cfg.pin_miso   = 39;        // Touch DO / MISO
      cfg.pin_cs     = 33;        // Touch CS
      cfg.bus_shared = false;
      cfg.x_min      = 185;       // Calibration from Launcher CYD28_TouchR_CAL_XMIN
      cfg.x_max      = 3700;      // Calibration from Launcher CYD28_TouchR_CAL_XMAX
      cfg.y_min      = 280;       // Calibration from Launcher CYD28_TouchR_CAL_YMIN
      cfg.y_max      = 3850;      // Calibration from Launcher CYD28_TouchR_CAL_YMAX
      cfg.offset_rotation = 0;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

// Onboard Hardware Pins
#define CYD_LED_RED     4    // Active LOW (LOW = ON)
#define CYD_LED_GREEN   16   // Active LOW
#define CYD_LED_BLUE    17   // Active LOW
#define CYD_LDR_PIN     34   // Ambient Light Sensor
