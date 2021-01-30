/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
extern float Madhumedad;  //datos humedad madera de main
extern float SStemperature;   //datos humedad y temp de main
extern float SShumidity;
extern float SStemperature2;    //datos humedad y temp post madera
extern float SShumidity2;
extern float SSdewpoint;
extern float SSdewpoint2;

#ifndef ULTRALCD_IMPL_HD44780_H
#define ULTRALCD_IMPL_HD44780_H

/**
 * Implementation of the LCD display routines for a Hitachi HD44780 display.
 * These are the most common LCD character displays.
 */

#include "utility.h"
#include "duration_t.h"
/*
#if ENABLED(AUTO_BED_LEVELING_UBL)
  #include "ubl.h"

  #if ENABLED(ULTIPANEL)
    #define ULTRA_X_PIXELS_PER_CHAR    5
    #define ULTRA_Y_PIXELS_PER_CHAR    8
    #define ULTRA_COLUMNS_FOR_MESH_MAP 7
    #define ULTRA_ROWS_FOR_MESH_MAP    4

    #define N_USER_CHARS    8

    #define TOP_LEFT      _BV(0)
    #define TOP_RIGHT     _BV(1)
    #define LOWER_LEFT    _BV(2)
    #define LOWER_RIGHT   _BV(3)
  #endif
#endif
*/
extern volatile uint8_t buttons;  //an extended version of the last checked buttons in a bit array.

////////////////////////////////////
// Setup button and encode mappings for each panel (into 'buttons' variable
//
// This is just to map common functions (across different panels) onto the same
// macro name. The mapping is independent of whether the button is directly connected or
// via a shift/i2c register.

#if ENABLED(ULTIPANEL)

  //
  // Setup other button mappings of each panel
  //
  #if ENABLED(LCD_I2C_VIKI)
    #define B_I2C_BTN_OFFSET 3 // (the first three bit positions reserved for EN_A, EN_B, EN_C)

    // button and encoder bit positions within 'buttons'
    #define B_LE (BUTTON_LEFT   << B_I2C_BTN_OFFSET)    // The remaining normalized buttons are all read via I2C
    #define B_UP (BUTTON_UP     << B_I2C_BTN_OFFSET)
    #define B_MI (BUTTON_SELECT << B_I2C_BTN_OFFSET)
    #define B_DW (BUTTON_DOWN   << B_I2C_BTN_OFFSET)
    #define B_RI (BUTTON_RIGHT  << B_I2C_BTN_OFFSET)

    #undef LCD_CLICKED
    #if BUTTON_EXISTS(ENC)
      // the pause/stop/restart button is connected to BTN_ENC when used
      #define B_ST (EN_C)                            // Map the pause/stop/resume button into its normalized functional name
      #define LCD_CLICKED (buttons & (B_MI|B_RI|B_ST)) // pause/stop button also acts as click until we implement proper pause/stop.
    #else
      #define LCD_CLICKED (buttons & (B_MI|B_RI))
    #endif

    // I2C buttons take too long to read inside an interrupt context and so we read them during lcd_update
    #define LCD_HAS_SLOW_BUTTONS

  #elif ENABLED(LCD_I2C_PANELOLU2)

    #if !BUTTON_EXISTS(ENC) // Use I2C if not directly connected to a pin

      #define B_I2C_BTN_OFFSET 3 // (the first three bit positions reserved for EN_A, EN_B, EN_C)

      #define B_MI (PANELOLU2_ENCODER_C << B_I2C_BTN_OFFSET) // requires LiquidTWI2 library v1.2.3 or later

      #undef LCD_CLICKED
      #define LCD_CLICKED (buttons & B_MI)

      // I2C buttons take too long to read inside an interrupt context and so we read them during lcd_update
      #define LCD_HAS_SLOW_BUTTONS

    #endif

  #elif DISABLED(NEWPANEL) // old style ULTIPANEL
    // Shift register bits correspond to buttons:
    #define BL_LE 7   // Left
    #define BL_UP 6   // Up
    #define BL_MI 5   // Middle
    #define BL_DW 4   // Down
    #define BL_RI 3   // Right
    #define BL_ST 2   // Red Button
    #define B_LE (_BV(BL_LE))
    #define B_UP (_BV(BL_UP))
    #define B_MI (_BV(BL_MI))
    #define B_DW (_BV(BL_DW))
    #define B_RI (_BV(BL_RI))
    #define B_ST (_BV(BL_ST))
    #define LCD_CLICKED (buttons & (B_MI|B_ST))
  #endif

#endif // ULTIPANEL

////////////////////////////////////
// Create LCD class instance and chipset-specific information
#if ENABLED(LCD_I2C_TYPE_PCF8575)
  // note: these are register mapped pins on the PCF8575 controller not Arduino pins
  #define LCD_I2C_PIN_BL  3
  #define LCD_I2C_PIN_EN  2
  #define LCD_I2C_PIN_RW  1
  #define LCD_I2C_PIN_RS  0
  #define LCD_I2C_PIN_D4  4
  #define LCD_I2C_PIN_D5  5
  #define LCD_I2C_PIN_D6  6
  #define LCD_I2C_PIN_D7  7

  #include <Wire.h>
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C
  LCD_CLASS lcd(LCD_I2C_ADDRESS, LCD_I2C_PIN_EN, LCD_I2C_PIN_RW, LCD_I2C_PIN_RS, LCD_I2C_PIN_D4, LCD_I2C_PIN_D5, LCD_I2C_PIN_D6, LCD_I2C_PIN_D7);

#elif ENABLED(LCD_I2C_TYPE_MCP23017)
  //for the LED indicators (which maybe mapped to different things in lcd_implementation_update_indicators())
  #define LED_A 0x04 //100
  #define LED_B 0x02 //010
  #define LED_C 0x01 //001

  #define LCD_HAS_STATUS_INDICATORS

  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2
  #if ENABLED(DETECT_DEVICE)
    LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
    LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif

#elif ENABLED(LCD_I2C_TYPE_MCP23008)
  #include <Wire.h>
  #include <LiquidTWI2.h>
  #define LCD_CLASS LiquidTWI2
  #if ENABLED(DETECT_DEVICE)
    LCD_CLASS lcd(LCD_I2C_ADDRESS, 1);
  #else
    LCD_CLASS lcd(LCD_I2C_ADDRESS);
  #endif

#elif ENABLED(LCD_I2C_TYPE_PCA8574)
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C
  LCD_CLASS lcd(LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT);

// 2 wire Non-latching LCD SR from:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!shiftregister-connection
#elif ENABLED(SR_LCD_2W_NL)
  extern "C" void __cxa_pure_virtual() { while (1); }
  #include <LCD.h>
  #include <LiquidCrystal_SR.h>
  #define LCD_CLASS LiquidCrystal_SR
  #if PIN_EXISTS(SR_STROBE)
    LCD_CLASS lcd(SR_DATA_PIN, SR_CLK_PIN, SR_STROBE_PIN);
  #else
    LCD_CLASS lcd(SR_DATA_PIN, SR_CLK_PIN);
  #endif
#elif ENABLED(LCM1602)
  #include <Wire.h>
  #include <LCD.h>
  #include <LiquidCrystal_I2C.h>
  #define LCD_CLASS LiquidCrystal_I2C
  LCD_CLASS lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#else
  // Standard directly connected LCD implementations
  #include <LiquidCrystal.h>
  #define LCD_CLASS LiquidCrystal
  LCD_CLASS lcd(LCD_PINS_RS, LCD_PINS_ENABLE, LCD_PINS_D4, LCD_PINS_D5, LCD_PINS_D6, LCD_PINS_D7); //RS,Enable,D4,D5,D6,D7
#endif

#include "utf_mapper.h"

#if ENABLED(LCD_PROGRESS_BAR)
  static millis_t progress_bar_ms = 0;     // Start millis of the current progress bar cycle
  #if PROGRESS_MSG_EXPIRE > 0
    static millis_t expire_status_ms = 0;  // millis at which to expire the status message
  #endif
  #define LCD_STR_PROGRESS  "\x03\x04\x05"
#endif

#if ENABLED(LCD_HAS_STATUS_INDICATORS)
  static void lcd_implementation_update_indicators();
#endif

static void createChar_P(const char c, const byte * const ptr) {
  byte temp[8];
  for (uint8_t i = 0; i < 8; i++)
    temp[i] = pgm_read_byte(&ptr[i]);
  lcd.createChar(c, temp);
}

#define CHARSET_MENU 0
#define CHARSET_INFO 1
#define CHARSET_BOOT 2

static void lcd_set_custom_characters(
  #if ENABLED(LCD_PROGRESS_BAR) || ENABLED(SHOW_BOOTSCREEN)
    const uint8_t screen_charset=CHARSET_INFO
  #endif
) {
  // CHARSET_BOOT
  #if ENABLED(SHOW_BOOTSCREEN)
    const static PROGMEM byte corner[4][8] = { {
      B00000,
      B00000,
      B00000,
      B00000,
      B00001,
      B00010,
      B00100,
      B00100
    }, {
      B00000,
      B00000,
      B00000,
      B11100,
      B11100,
      B01100,
      B00100,
      B00100
    }, {
      B00100,
      B00010,
      B00001,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    }, {
      B00100,
      B01000,
      B10000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    } };
  #endif // SHOW_BOOTSCREEN

  // CHARSET_INFO
  const static PROGMEM byte bedTemp[8] = {
    B00000,
    B11111,
    B10101,
    B10001,
    B10101,
    B11111,
    B00000,
    B00000
  };

  const static PROGMEM byte degree[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000
  };

  const static PROGMEM byte thermometer[8] = {
    B00100,
    B01010,
    B01010,
    B01010,
    B01010,
    B10001,
    B10001,
    B01110
  };

  const static PROGMEM byte uplevel[8] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B11100,
    B00000,
    B00000,
    B00000
  };

/*  const static PROGMEM byte feedrate[8] = {
    B11100,
    B10000,
    B11000,
    B10111,
    B00101,
    B00110,
    B00101,
    B00000
  };
*/
//Velocidad Bomba
  const static PROGMEM byte feedrate[8] = {
    B10100,
    B10100,
    B01110,
    B00101,
    B00111,
    B00101,
    B00110,
    B00000
  };

  const static PROGMEM byte clock[8] = {
    B00000,
    B01110,
    B10011,
    B10101,
    B10001,
    B01110,
    B00000,
    B00000
  };

  #if ENABLED(LCD_PROGRESS_BAR)

    // CHARSET_INFO
    const static PROGMEM byte progress[3][8] = { {
      B00000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B10000,
      B00000
    }, {
      B00000,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B10100,
      B00000
    }, {
      B00000,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B10101,
      B00000
    } };

  #endif // LCD_PROGRESS_BAR

  #if ENABLED(SDSUPPORT)

    // CHARSET_MENU
    const static PROGMEM byte refresh[8] = {
      B00000,
      B00110,
      B11001,
      B11000,
      B00011,
      B10011,
      B01100,
      B00000,
    };
    const static PROGMEM byte folder[8] = {
      B00000,
      B11100,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,
      B00000
    };

  #endif // SDSUPPORT

  #if ENABLED(SHOW_BOOTSCREEN)
    // Set boot screen corner characters
    if (screen_charset == CHARSET_BOOT) {
      for (uint8_t i = 4; i--;)
        createChar_P(i, corner[i]);
    }
    else
  #endif
    { // Info Screen uses 5 special characters
      createChar_P(LCD_BEDTEMP_CHAR, bedTemp);
      createChar_P(LCD_DEGREE_CHAR, degree);
      createChar_P(LCD_STR_THERMOMETER[0], thermometer);
      createChar_P(LCD_FEEDRATE_CHAR, feedrate);
      createChar_P(LCD_CLOCK_CHAR, clock);

      #if ENABLED(LCD_PROGRESS_BAR)
        if (screen_charset == CHARSET_INFO) { // 3 Progress bar characters for info screen
          for (int16_t i = 3; i--;)
            createChar_P(LCD_STR_PROGRESS[i], progress[i]);
        }
        else
      #endif
        {
          createChar_P(LCD_UPLEVEL_CHAR, uplevel);
          #if ENABLED(SDSUPPORT)
            // SD Card sub-menu special characters
            createChar_P(LCD_STR_REFRESH[0], refresh);
            createChar_P(LCD_STR_FOLDER[0], folder);
          #endif
        }
    }
}

static void lcd_implementation_init(
  #if ENABLED(LCD_PROGRESS_BAR)
    const uint8_t screen_charset=CHARSET_INFO
  #endif
) {

  #if ENABLED(LCD_I2C_TYPE_PCF8575)
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    #ifdef LCD_I2C_PIN_BL
      lcd.setBacklightPin(LCD_I2C_PIN_BL, POSITIVE);
      lcd.setBacklight(HIGH);
    #endif

  #elif ENABLED(LCD_I2C_TYPE_MCP23017)
    lcd.setMCPType(LTI_TYPE_MCP23017);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
    lcd_implementation_update_indicators();

  #elif ENABLED(LCD_I2C_TYPE_MCP23008)
    lcd.setMCPType(LTI_TYPE_MCP23008);
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);

  #elif ENABLED(LCD_I2C_TYPE_PCA8574)
    lcd.init();
    lcd.backlight();

  #else
    lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  #endif

  lcd_set_custom_characters(
    #if ENABLED(LCD_PROGRESS_BAR)
      screen_charset
    #endif
  );

  lcd.clear();
}

void lcd_implementation_clear() { lcd.clear(); }
void lcd_print(const char c) { charset_mapper(c); }
void lcd_print(const char *str) { while (*str) lcd.print(*str++); }
void lcd_printPGM(const char *str) { while (const char c = pgm_read_byte(str)) lcd.print(c), ++str; }

void lcd_print_utf(const char *str, uint8_t n=LCD_WIDTH) {
  char c;
  while (n && (c = *str)) n -= charset_mapper(c), ++str;
}

void lcd_printPGM_utf(const char *str, uint8_t n=LCD_WIDTH) {
  char c;
  while (n && (c = pgm_read_byte(str))) n -= charset_mapper(c), ++str;
}

#if ENABLED(SHOW_BOOTSCREEN)

  void lcd_erase_line(const int16_t line) {
    lcd.setCursor(0, line);
    for (uint8_t i = LCD_WIDTH + 1; --i;)
      lcd.write(' ');
  }

  // Scroll the PSTR 'text' in a 'len' wide field for 'time' milliseconds at position col,line
  void lcd_scroll(const int16_t col, const int16_t line, const char* const text, const int16_t len, const int16_t time) {
    uint8_t slen = utf8_strlen_P(text);
    if (slen < len) {
      // Fits into,
      lcd.setCursor(col, line);
      lcd_printPGM_utf(text, len);
      while (slen < len) {
        lcd.write(' ');
        ++slen;
      }
      safe_delay(time);
    }
    else {
      const char* p = text;
      int dly = time / MAX(slen, 1);
      for (uint8_t i = 0; i <= slen; i++) {

        // Go to the correct place
        lcd.setCursor(col, line);

        // Print the text
        lcd_printPGM_utf(p, len);

        // Fill with spaces
        uint8_t ix = slen - i;
        while (ix < len) {
          lcd.write(' ');
          ++ix;
        }

        // Delay
        safe_delay(dly);

        // Advance to the next UTF8 valid position
        p++;
        while (!START_OF_UTF8_CHAR(pgm_read_byte(p))) p++;
      }
    }
  }

  static void logo_lines(const char* const extra) {
    int16_t indent = (LCD_WIDTH - 8 - utf8_strlen_P(extra)) / 2;
    lcd.setCursor(indent, 0); lcd.print('\x00'); lcd_printPGM(PSTR( "------------" ));  lcd.write('\x01');
    lcd.setCursor(indent, 1);                    lcd_printPGM(PSTR("| KILN 01 MS |"));  //lcd_printPGM(extra);
    lcd.setCursor(indent, 2); lcd.write('\x02'); lcd_printPGM(PSTR( "------------" ));  lcd.write('\x03');
    delay (2000);
  }

  void lcd_bootscreen() {
    lcd_set_custom_characters(CHARSET_BOOT);
    lcd.clear();

    #define LCD_EXTRA_SPACE (LCD_WIDTH-8)

    #define CENTER_OR_SCROLL(STRING,DELAY) \
      lcd_erase_line(3); \
      if (strlen(STRING) <= LCD_WIDTH) { \
        lcd.setCursor((LCD_WIDTH - utf8_strlen_P(PSTR(STRING))) / 2, 3); \
        lcd_printPGM_utf(PSTR(STRING)); \
        safe_delay(DELAY); \
      } \
      else { \
        lcd_scroll(0, 3, PSTR(STRING), LCD_WIDTH, DELAY); \
      }

    #ifdef STRING_SPLASH_LINE1
      //
      // Show the Marlin logo with splash line 1
      //
      if (LCD_EXTRA_SPACE >= strlen(STRING_SPLASH_LINE1) + 1) {
        //
        // Show the Marlin logo, splash line1, and splash line 2
        //
        logo_lines(PSTR(" " STRING_SPLASH_LINE1));
        #ifdef STRING_SPLASH_LINE2
          CENTER_OR_SCROLL(STRING_SPLASH_LINE2, 2000);
        #else
          safe_delay(2000);
        #endif
      }
      else {
        //
        // Show the Marlin logo with splash line 1
        // After a delay show splash line 2, if it exists
        //
        #ifdef STRING_SPLASH_LINE2
          #define _SPLASH_WAIT_1 1500
        #else
          #define _SPLASH_WAIT_1 2000
        #endif
        logo_lines(PSTR(""));
        CENTER_OR_SCROLL(STRING_SPLASH_LINE1, _SPLASH_WAIT_1);
        #ifdef STRING_SPLASH_LINE2
          CENTER_OR_SCROLL(STRING_SPLASH_LINE2, 1500);
        #endif
      }
    #elif defined(STRING_SPLASH_LINE2)
      //
      // Show splash line 2 only, alongside the logo if possible
      //
      if (LCD_EXTRA_SPACE >= strlen(STRING_SPLASH_LINE2) + 1) {
        logo_lines(PSTR(" " STRING_SPLASH_LINE2));
        safe_delay(2000);
      }
      else {
        logo_lines(PSTR(""));
        CENTER_OR_SCROLL(STRING_SPLASH_LINE2, 2000);
      }
    #else
      //
      // Show only the Marlin logo
      //
      logo_lines(PSTR(""));
      safe_delay(2000);
    #endif

    lcd.clear();
    safe_delay(100);
    lcd_set_custom_characters();
    lcd.clear();
  }

#endif // SHOW_BOOTSCREEN

void lcd_kill_screen() {
  lcd.setCursor(0, 0);
  lcd_print_utf(lcd_status_message);
  #if LCD_HEIGHT < 4
    lcd.setCursor(0, 2);
  #else
    lcd.setCursor(0, 2);
    lcd_printPGM_utf(PSTR(MSG_HALTED));
    lcd.setCursor(0, 3);
  #endif
  lcd_printPGM_utf(PSTR(MSG_PLEASE_RESET));
}

//
// Before homing, blink '123' <-> '???'.
// Homed but unknown... '123' <-> '   '.
// Homed and known, display constantly.
//
FORCE_INLINE void _draw_axis_value(const AxisEnum axis, const char *value, const bool blink) {
  lcd_print('X' + uint8_t(axis));
  if (blink)
    lcd.print(value);
  else {
    if (!TEST(axis_homed, axis))
      while (const char c = *value++) lcd_print(c <= '.' ? c : '?');
    else {
      #if DISABLED(HOME_AFTER_DEACTIVATE) && DISABLED(DISABLE_REDUCED_ACCURACY_WARNING)
        if (!TEST(axis_known_position, axis))
          lcd_printPGM(axis == Z_AXIS ? PSTR("      ") : PSTR("    "));
        else
      #endif
          lcd.print(value);
    }
  }
}
/*///////////////////////////////////////////////////////////////////////
 * Busca datos ,conforma linea de texto e imprime muy limitado
 */
FORCE_INLINE void _draw_heater_status(const int8_t heater, const char prefix, const bool blink) {
  #if HAS_HEATED_BED
    const bool isBed = heater < 0;
    const float t1 = (isBed ? thermalManager.degBed()       : thermalManager.degHotend(heater)),
                t2 = (isBed ? thermalManager.degTargetBed() : thermalManager.degTargetHotend(heater));
  #else
    const float t1 = thermalManager.degHotend(heater), t2 = thermalManager.degTargetHotend(heater);
  #endif

  if (prefix >= 0) lcd.print(prefix);

  lcd.print(itostr3(t1 + 0.5));
//  lcd.write('/');

  #if !HEATER_IDLE_HANDLER
    UNUSED(blink);
  #else
    const bool is_idle = (
      #if HAS_HEATED_BED
        isBed ? thermalManager.is_bed_idle() :
      #endif
      thermalManager.is_heater_idle(heater)
    );

    if (!blink && is_idle) {
      lcd.write(' ');
      if (t2 >= 10) lcd.write(' ');
      if (t2 >= 100) lcd.write(' ');
    }
    else
  #endif
      lcd.print(itostr3left(t2 + 0.5));

  if (prefix >= 0) {
    lcd.print((char)LCD_DEGREE_CHAR);
    lcd.write(' ');
    if (t2 < 10) lcd.write(' ');
  }
}
///////////////////////////////////////////////////////////////////////

#if ENABLED(LCD_PROGRESS_BAR)

  inline void lcd_draw_progress_bar(const uint8_t percent) {
    const int16_t tix = (int16_t)(percent * (LCD_WIDTH) * 3) / 100,
              cel = tix / 3,
              rem = tix % 3;
    uint8_t i = LCD_WIDTH;
    char msg[LCD_WIDTH + 1], b = ' ';
    msg[LCD_WIDTH] = '\0';
    while (i--) {
      if (i == cel - 1)
        b = LCD_STR_PROGRESS[2];
      else if (i == cel && rem != 0)
        b = LCD_STR_PROGRESS[rem - 1];
      msg[i] = b;
    }
    lcd.print(msg);
  }

#endif // LCD_PROGRESS_BAR

/**
Possible status screens:
16x2   |000/000 B000/000|
       |0123456789012345|

16x4   |000/000 B000/000|
       |SD100%  Z 000.00|
       |F100%     T--:--|
       |0123456789012345|

20x2   |T000/000D B000/000D |
       |01234567890123456789|

20x4   |T000/000D B000/000D |
       |X 000 Y 000 Z 000.00|
       |F100%  SD100% T--:--|
       |01234567890123456789|

20x4   |T000/000D B000/000D |
       |T000/000D   Z 000.00|
       |F100%  SD100% T--:--|
       |01234567890123456789|
*/
////////////////////////////////////
//Pantalla principal
//
////////////////////////////////////////////////////////////////////////////////
static void lcd_implementation_status_screen() {
  const bool blink = lcd_blink();

  //
  // Line 1
  //

  lcd.setCursor(0, 0);


//  #if  LCD_WIDTH >= 20

    //
    // Hotend 0 Temperature
    //
//   _draw_heater_status(0, LCD_STR_THERMOMETER[0], blink);
//lcd.print("Pre ");
lcd.print((char)LCD_STR_THERMOMETER[0]);
lcd.print(String(SStemperature,1));
lcd.print((char)LCD_DEGREE_CHAR);
lcd.print(" H");
lcd.print(String(SShumidity,0));
lcd.print("%");
//lcd.print(String(SSdewpoint,0)); 
//lcd.print((char)LCD_DEGREE_CHAR);
////////////////////////////////////////////////////////////////////////////////////////Aqui cambiar tiempo ciclo KILN YA
   char buffer[10];
    duration_t elapsed = print_job_timer.duration();
    uint8_t len = elapsed.toDigital(buffer);

//    lcd.setCursor(LCD_WIDTH - len - 1, 2);
    lcd.setCursor(14, 0);
    lcd.print((char)LCD_CLOCK_CHAR);
    lcd_print(buffer);  //El bueno
//    lcd.print(len);   //El malo
    
  //
  // Line 2
  //

  lcd.setCursor(0, 1);
//lcd.print("Post");
lcd.print((char)LCD_STR_THERMOMETER[0]);
lcd.print(String(SStemperature2,1));
lcd.print((char)LCD_DEGREE_CHAR);
lcd.print(" H");
lcd.print(String(SShumidity2,0));
lcd.print("%  Dew ");    //
lcd.print(String(SSdewpoint2,0)); 
lcd.print((char)LCD_DEGREE_CHAR);


////////////////////////////////////////////////////////////////////////////////////////Aqui hay espacio para cambiar Z por lo que falte (presion agua
//
// Line 3
//
        lcd.setCursor(0, 2);
/*/lcd.print((char)LCD_BEDTEMP_CHAR);
extern float Madhumedad;  //datos humedad madera de main
extern float SStemperature;   //datos humedad y temp de main
extern float SShumidity;
extern float SStemperature2;
extern float SShumidity2;
*/
lcd.print("Amb");
lcd.print(itostr3(thermalManager.degBed() + 0.5));  //usando bed 0 para ambiente
lcd.print((char)LCD_DEGREE_CHAR);
lcd.print(" Madera ");
lcd.print(String(Madhumedad,1));
lcd.print("% ");
  #if LCD_HEIGHT > 3
////////////////////////////////////////////////////////////////////////////////////////Aqui cambiar feedrate por velocidad de la bomba

 //   lcd.setCursor(0, 2);
//    lcd.print((char)LCD_FEEDRATE_CHAR);
//    lcd.print(itostr3(feedrate_percentage));
//    lcd.write('%');

 //   #if LCD_WIDTH >= 20 && ENABLED(SDSUPPORT)
////////////////////////////////////////////////////////////////////////////////////////Aqui cambiar cantidad de memoria usada en tarjeta sd(regstro eventos)
  //    lcd.setCursor(6, 2);
  //    lcd_printPGM(PSTR("Cic"));
 //     if (IS_SD_PRINTING)
  //      lcd.print(itostr3(card.percentDone()));
  //    else
  //      lcd_printPGM(PSTR("---"));
  //    lcd.write('%');

 //   #endif // LCD_WIDTH >= 20 && SDSUPPORT


  #endif // LCD_HEIGHT > 3

  //
  // Last Line
  // Status Message (which may be a Progress Bar or Filament display)
  //

  lcd.setCursor(0, LCD_HEIGHT - 1);

  #if ENABLED(LCD_PROGRESS_BAR)

    // Draw the progress bar if the message has shown long enough
    // or if there is no message set.
    #if DISABLED(LCD_SET_PROGRESS_MANUALLY)
      const uint8_t progress_bar_percent = card.percentDone();
    #endif
    if (progress_bar_percent > 2 && (ELAPSED(millis(), progress_bar_ms + PROGRESS_BAR_MSG_TIME) || !lcd_status_message[0]))
      return lcd_draw_progress_bar(progress_bar_percent);

  #elif ENABLED(FILAMENT_LCD_DISPLAY) && ENABLED(SDSUPPORT)

    // Show Filament Diameter and Volumetric Multiplier %
    // After allowing lcd_status_message to show for 5 seconds
    if (ELAPSED(millis(), previous_lcd_status_ms + 5000UL)) {
      lcd_printPGM(PSTR("Dia "));
      lcd.print(ftostr12ns(filament_width_meas));
      lcd_printPGM(PSTR(" V"));
      lcd.print(itostr3(100.0 * (
          parser.volumetric_enabled
            ? planner.volumetric_area_nominal / planner.volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]
            : planner.volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM]
        )
      ));
      lcd.write('%');
      return;
    }

  #endif // FILAMENT_LCD_DISPLAY && SDSUPPORT

  #if ENABLED(STATUS_MESSAGE_SCROLLING)
    static bool last_blink = false;

    // Get the UTF8 character count of the string
    uint8_t slen = utf8_strlen(lcd_status_message);

    // If the string fits into the LCD, just print it and do not scroll it
    if (slen <= LCD_WIDTH) {

      // The string isn't scrolling and may not fill the screen
      lcd_print_utf(lcd_status_message);

      // Fill the rest with spaces
      while (slen < LCD_WIDTH) {
        lcd.write(' ');
        ++slen;
      }
    }
    else {
      // String is larger than the available space in screen.

      // Get a pointer to the next valid UTF8 character
      const char *stat = lcd_status_message + status_scroll_offset;

      // Get the string remaining length
      const uint8_t rlen = utf8_strlen(stat);

      // If we have enough characters to display
      if (rlen >= LCD_WIDTH) {
        // The remaining string fills the screen - Print it
        lcd_print_utf(stat, LCD_WIDTH);
      }
      else {

        // The remaining string does not completely fill the screen
        lcd_print_utf(stat, LCD_WIDTH);               // The string leaves space
        uint8_t chars = LCD_WIDTH - rlen;             // Amount of space left in characters

        lcd.write('.');                               // Always at 1+ spaces left, draw a dot
        if (--chars) {                                // Draw a second dot if there's space
          lcd.write('.');
          if (--chars)
            lcd_print_utf(lcd_status_message, chars); // Print a second copy of the message
        }
      }
      if (last_blink != blink) {
        last_blink = blink;

        // Adjust by complete UTF8 characters
        if (status_scroll_offset < slen) {
          status_scroll_offset++;
          while (!START_OF_UTF8_CHAR(lcd_status_message[status_scroll_offset]))
            status_scroll_offset++;
        }
        else
          status_scroll_offset = 0;
      }
    }
  #else
    UNUSED(blink);

    // Get the UTF8 character count of the string
    uint8_t slen = utf8_strlen(lcd_status_message);

    // Just print the string to the LCD
    lcd_print_utf(lcd_status_message, LCD_WIDTH);

    // Fill the rest with spaces if there are missing spaces
    while (slen < LCD_WIDTH) {
      lcd.write(' ');
      ++slen;
    }
  #endif
}

#if ENABLED(ULTIPANEL)

  #if ENABLED(ADVANCED_PAUSE_FEATURE)

    static void lcd_implementation_hotend_status(const uint8_t row, const uint8_t extruder=active_extruder) {
      if (row < LCD_HEIGHT) {
        lcd.setCursor(LCD_WIDTH - 9, row);
        _draw_heater_status(extruder, LCD_STR_THERMOMETER[0], lcd_blink());
      }
    }

  #endif // ADVANCED_PAUSE_FEATURE

  static void lcd_implementation_drawmenu_static(const uint8_t row, const char* pstr, const bool center=true, const bool invert=false, const char *valstr=NULL) {
    UNUSED(invert);
    char c;
    int8_t n = LCD_WIDTH;
    lcd.setCursor(0, row);
    if (center && !valstr) {
      int8_t pad = (LCD_WIDTH - utf8_strlen_P(pstr)) / 2;
      while (--pad >= 0) { lcd.write(' '); n--; }
    }
    while (n > 0 && (c = pgm_read_byte(pstr))) {
      n -= charset_mapper(c);
      pstr++;
    }
    if (valstr) while (n > 0 && (c = *valstr)) {
      n -= charset_mapper(c);
      valstr++;
    }
    while (n-- > 0) lcd.write(' ');
  }

////////////////////////////////////
//drawmenu_generic
//
////////////////////////////////////////////////////////////////////////////////
  static void lcd_implementation_drawmenu_generic(const bool sel, const uint8_t row, const char* pstr, const char pre_char, const char post_char) {
    char c;
    uint8_t n = LCD_WIDTH - 2;
    lcd.setCursor(0, row);
    lcd.print(sel ? pre_char : ' ');
    while ((c = pgm_read_byte(pstr)) && n > 0) {
      n -= charset_mapper(c);
      pstr++;
    }
    while (n--) lcd.write(' ');
    lcd.print(post_char);
  }

////////////////////////////////////
//drawedit
//
////////////////////////////////////////////////////////////////////////////////
static void lcd_implementation_drawmenu_setting_edit_generic(const bool sel, const uint8_t row, const char* pstr, const char pre_char, const char* const data) {
    char c;
    uint8_t n = LCD_WIDTH - 2 - utf8_strlen(data);
    lcd.setCursor(0, row);
    lcd.print(sel ? pre_char : ' ');
    while ((c = pgm_read_byte(pstr)) && n > 0) {
      n -= charset_mapper(c);
      pstr++;
    }
    lcd.write(':');
    while (n--) lcd.write(' ');
    lcd_print(data);
  }
  static void lcd_implementation_drawmenu_setting_edit_generic_P(const bool sel, const uint8_t row, const char* pstr, const char pre_char, const char* const data) {
    char c;
    uint8_t n = LCD_WIDTH - 2 - utf8_strlen_P(data);
    lcd.setCursor(0, row);
    lcd.print(sel ? pre_char : ' ');
    while ((c = pgm_read_byte(pstr)) && n > 0) {
      n -= charset_mapper(c);
      pstr++;
    }
    lcd.write(':');
    while (n--) lcd.write(' ');
    lcd_printPGM(data);
  }

  #define DRAWMENU_SETTING_EDIT_GENERIC(_src) lcd_implementation_drawmenu_setting_edit_generic(sel, row, pstr, '>', _src)
  #define DRAW_BOOL_SETTING(sel, row, pstr, data) lcd_implementation_drawmenu_setting_edit_generic_P(sel, row, pstr, '>', (*(data))?PSTR(MSG_ON):PSTR(MSG_OFF))

  void lcd_implementation_drawedit(const char* pstr, const char* const value=NULL) {
    lcd.setCursor(1, 1);
    lcd_printPGM_utf(pstr);
    if (value != NULL) {
      lcd.write(':');
      const uint8_t valrow = (utf8_strlen_P(pstr) + 1 + utf8_strlen(value) + 1) > (LCD_WIDTH - 2) ? 2 : 1; // Value on the next row if it won't fit
      lcd.setCursor((LCD_WIDTH - 1) - (utf8_strlen(value) + 1), valrow);                                  // Right-justified, padded by spaces
      lcd.write(' ');                                                                                     // overwrite char if value gets shorter
      lcd_print(value);
    }
  }
////////////////////////////////////
//drawmenu SD card
//
////////////////////////////////////////////////////////////////////////////////

  #if ENABLED(SDSUPPORT)

    static void lcd_implementation_drawmenu_sd(const bool sel, const uint8_t row, const char* const pstr, CardReader& theCard, const uint8_t concat, const char post_char) {
      UNUSED(pstr);
      lcd.setCursor(0, row);
      lcd.print(sel ? '>' : ' ');

      uint8_t n = LCD_WIDTH - concat;
      const char *outstr = theCard.longest_filename();
      if (theCard.longFilename[0]) {
        #if ENABLED(SCROLL_LONG_FILENAMES)
          if (sel) {
            uint8_t name_hash = row;
            for (uint8_t l = FILENAME_LENGTH; l--;)
              name_hash = ((name_hash << 1) | (name_hash >> 7)) ^ theCard.filename[l];  // rotate, xor
            if (filename_scroll_hash != name_hash) {                            // If the hash changed...
              filename_scroll_hash = name_hash;                                 // Save the new hash
              filename_scroll_max = MAX(0, utf8_strlen(theCard.longFilename) - n);  // Update the scroll limit
              filename_scroll_pos = 0;                                          // Reset scroll to the start
              lcd_status_update_delay = 8;                                      // Don't scroll right away
            }
            outstr += filename_scroll_pos;
          }
        #else
          theCard.longFilename[n] = '\0'; // cutoff at screen edge
        #endif
      }

      char c;
      while (n && (c = *outstr)) {
        n -= charset_mapper(c);
        ++outstr;
      }
      while (n) { --n; lcd.write(' '); }

      lcd.print(post_char);
    }

    static void lcd_implementation_drawmenu_sdfile(const bool sel, const uint8_t row, const char* pstr, CardReader& theCard) {
      lcd_implementation_drawmenu_sd(sel, row, pstr, theCard, 2, ' ');
    }

    static void lcd_implementation_drawmenu_sddirectory(const bool sel, const uint8_t row, const char* pstr, CardReader& theCard) {
      lcd_implementation_drawmenu_sd(sel, row, pstr, theCard, 2, LCD_STR_FOLDER[0]);
    }

  #endif // SDSUPPORT

  #define lcd_implementation_drawmenu_back(sel, row, pstr, dummy) lcd_implementation_drawmenu_generic(sel, row, pstr, LCD_UPLEVEL_CHAR, LCD_UPLEVEL_CHAR)
  #define lcd_implementation_drawmenu_submenu(sel, row, pstr, data) lcd_implementation_drawmenu_generic(sel, row, pstr, '>', LCD_STR_ARROW_RIGHT[0])
  #define lcd_implementation_drawmenu_gcode(sel, row, pstr, gcode) lcd_implementation_drawmenu_generic(sel, row, pstr, '>', ' ')
  #define lcd_implementation_drawmenu_function(sel, row, pstr, data) lcd_implementation_drawmenu_generic(sel, row, pstr, '>', ' ')

  #if ENABLED(LCD_HAS_SLOW_BUTTONS)

    extern millis_t next_button_update_ms;

    static uint8_t lcd_implementation_read_slow_buttons() {
      #if ENABLED(LCD_I2C_TYPE_MCP23017)
        // Reading these buttons this is likely to be too slow to call inside interrupt context
        // so they are called during normal lcd_update
        uint8_t slow_bits = lcd.readButtons() << B_I2C_BTN_OFFSET;
        #if ENABLED(LCD_I2C_VIKI)
          if ((slow_bits & (B_MI | B_RI)) && PENDING(millis(), next_button_update_ms)) // LCD clicked
            slow_bits &= ~(B_MI | B_RI); // Disable LCD clicked buttons if screen is updated
        #endif // LCD_I2C_VIKI
        return slow_bits;
      #endif // LCD_I2C_TYPE_MCP23017
    }

  #endif // LCD_HAS_SLOW_BUTTONS

  #if ENABLED(LCD_HAS_STATUS_INDICATORS)

    static void lcd_implementation_update_indicators() {
      // Set the LEDS - referred to as backlights by the LiquidTWI2 library
      static uint8_t ledsprev = 0;
      uint8_t leds = 0;

      if (thermalManager.degTargetBed() > 0) leds |= LED_A;

      if (thermalManager.degTargetHotend(0) > 0) leds |= LED_B;

      #if FAN_COUNT > 0
        if (0
          #if HAS_FAN0
            || fanSpeeds[0]
          #endif
          #if HAS_FAN1
            || fanSpeeds[1]
          #endif
          #if HAS_FAN2
            || fanSpeeds[2]
          #endif
        ) leds |= LED_C;
      #endif // FAN_COUNT > 0

      #if HOTENDS > 1
        if (thermalManager.degTargetHotend(1) > 0) leds |= LED_C;
      #endif

      if (leds != ledsprev) {
        lcd.setBacklight(leds);
        ledsprev = leds;
      }
    }

  #endif // LCD_HAS_STATUS_INDICATORS


#endif // ULTIPANEL

#endif // ULTRALCD_IMPL_HD44780_H
