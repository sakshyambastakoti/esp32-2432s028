#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// ============================================================================
// LovyanGFX Hardware Configuration for ESP32-2432S028 (CYD / Cheap Yellow Display)
// Display: 2.8" TFT 240x320 SPI (ILI9341 / ST7789)
// Touch:   XPT2046 Resistive Touch on HSPI
// RGB LED: Pins 4 (R), 16 (G), 17 (B) - Active LOW
// LDR:     Pin 34 (Analog Light Sensor)
// ============================================================================

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void) {
    // 1. VSPI Bus for TFT LCD Display
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host   = VSPI_HOST; // SPI2
      cfg.spi_mode   = 0;
      cfg.freq_write = 40000000;  // 40MHz write speed
      cfg.freq_read  = 16000000;
      cfg.pin_sclk   = 14;        // TFT SCK
      cfg.pin_mosi   = 13;        // TFT MOSI
      cfg.pin_miso   = 12;        // TFT MISO
      cfg.pin_dc     = 2;         // TFT DC / RS
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    // 2. Display Panel ILI9341 (320x240 Native Landscape on CYD)
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 15;   // TFT CS
      cfg.pin_rst          = -1;   // Connected to EN
      cfg.panel_width      = 320;  // Native CYD panel width (rotated to 240x320 portrait via setRotation)
      cfg.panel_height     = 240;  // Native CYD panel height
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable         = false;
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

    // 4. Touch Screen Controller (XPT2046 on HSPI)
    {
      auto cfg = _touch_instance.config();
      cfg.pin_int    = -1;        // Touch IRQ (-1 for reliable polling on CYD)
      cfg.spi_host   = HSPI_HOST; // SPI3
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
