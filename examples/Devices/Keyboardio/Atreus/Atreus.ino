/* -*- mode: c++ -*-
 * Atreus -- Chrysalis-enabled Sketch for the Keyboardio Atreus
 * Copyright (C) 2018, 2019  Keyboard.io, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif

#include "Kaleidoscope.h"
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-Chords.h"
#include "Kaleidoscope-KeyTimings.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-SpaceCadet.h"



#define MO(n) ShiftToLayer(n)
#define TG(n) LockLayer(n)
#define UN(n) UnlockLayer(n)

enum {
  MACRO_QWERTY,
  MACRO_VERSION_INFO
};

#define Key_Exclamation LSHIFT(Key_1)
#define Key_At LSHIFT(Key_2)
#define Key_Hash LSHIFT(Key_3)
#define Key_Dollar LSHIFT(Key_4)
#define Key_Percent LSHIFT(Key_5)
#define Key_Caret LSHIFT(Key_6)
#define Key_And LSHIFT(Key_7)
#define Key_Star LSHIFT(Key_8)
#define Key_Plus LSHIFT(Key_Equals)

enum {
  NOLMTS,
  QWERTY,
  FUN,
  UPPER,
  SHORTCUT
};

/* *INDENT-OFF* */
KEYMAPS(
  [NOLMTS] = KEYMAP_STACKED
  (
       Key_Q   ,Key_W   ,Key_F       ,Key_R         ,Key_J
      ,Key_A   ,Key_T   ,Key_H       ,Key_E         ,Key_G
      ,Key_Z   ,Key_X   ,Key_D       ,Key_V         ,Key_B, Key_Backtick
      ,Key_Escape ,Key_Tab ,Key_LeftGui ,MO(FUN) ,Key_Space ,Key_LeftAlt

                     ,Key_Y     ,Key_U      ,Key_Semicolon     ,Key_L      ,Key_P
                     ,Key_C     ,Key_S      ,Key_N     ,Key_O      ,Key_I
       ,Key_Backslash,Key_K     ,Key_M      ,Key_Comma ,Key_Period ,Key_Slash
       ,Key_LeftControl  ,Key_LeftShift    ,Key_Backspace ,Key_Minus ,Key_RightAlt  ,Key_Enter
  ),

  [QWERTY] = KEYMAP_STACKED
  (
       Key_Q   ,Key_W   ,Key_E       ,Key_R         ,Key_T
      ,Key_A   ,Key_S   ,Key_D       ,Key_F         ,Key_G
      ,Key_Z   ,Key_X   ,Key_C       ,Key_V         ,Key_B, Key_Backtick
      ,Key_Escape ,Key_Tab ,Key_LeftGui ,MO(FUN) ,Key_Space ,Key_LeftAlt

                     ,Key_Y     ,Key_U      ,Key_I     ,Key_O      ,Key_P
                     ,Key_H     ,Key_J      ,Key_K     ,Key_L      ,Key_Semicolon
       ,Key_Backslash,Key_N     ,Key_M      ,Key_Comma ,Key_Period ,Key_Slash
       ,Key_LeftControl  ,Key_LeftShift    ,Key_Backspace ,Key_Minus ,Key_RightAlt  ,Key_Enter
  ),

  [FUN] = KEYMAP_STACKED
  (
       Key_Exclamation ,Key_At           ,Key_UpArrow   ,Key_Dollar           ,Key_Percent
      ,Key_LeftParen   ,Key_LeftArrow    ,Key_DownArrow ,Key_RightArrow       ,Key_RightParen
      ,Key_LeftBracket ,Key_RightBracket ,Key_Hash      ,Key_LeftCurlyBracket ,Key_RightCurlyBracket ,Key_Caret
      ,TG(UPPER)       ,Key_Insert       ,Key_LeftGui   ,MO(FUN)        ,Key_Space         ,Key_LeftAlt

                   ,Key_PageUp   ,Key_7 ,Key_8      ,Key_9 ,Key_Backspace
                   ,Key_PageDown ,Key_4 ,Key_5      ,Key_6 ,Key_Quote
      ,Key_And     ,Key_Star     ,Key_1 ,Key_2      ,Key_3 ,Key_Plus
      ,Key_LeftControl  ,Key_LeftShift    ,Key_Delete ,Key_Period ,Key_0  ,Key_Equals
   ),

  [UPPER] = KEYMAP_STACKED
  (
       Key_Insert            ,Key_Home                 ,Key_UpArrow   ,Key_End        ,Key_PageUp
      ,Key_Delete            ,Key_LeftArrow            ,Key_DownArrow ,Key_RightArrow ,Key_PageDown
      ,M(MACRO_VERSION_INFO) ,Consumer_VolumeIncrement ,XXX           ,XXX            ,___ ,TG(SHORTCUT)
      ,MoveToLayer(NOLMTS)   ,Consumer_VolumeDecrement ,___           ,MoveToLayer(QWERTY) ,___ ,___

                ,Key_UpArrow   ,Key_F7              ,Key_F8          ,Key_F9         ,Key_F10
                ,Key_DownArrow ,Key_F4              ,Key_F5          ,Key_F6         ,Key_F11
      ,___      ,XXX           ,Key_F1              ,Key_F2          ,Key_F3         ,Key_F12
      ,___      ,___           ,___                 ,Key_PrintScreen ,Key_ScrollLock ,Consumer_PlaySlashPause
   ),

  [SHORTCUT] = KEYMAP_STACKED
  (
       ___ ,___ ,___ ,___ ,___
      ,___ ,___ ,___ ,___ ,___
      ,Key_Z ,Key_X ,___ ,___ ,Key_B ,UN(SHORTCUT)
      ,___ ,___ ,___ ,___ ,___ ,___

      ,Key_Z ,Key_X ,___ ,___ ,___
      ,Key_H     ,Key_J      ,Key_K     ,Key_L      ,___
      ,___ ,___ ,___ ,___ ,___ ,___
      ,LCTRL(LSHIFT(Key_N)) ,Key_Space ,Key_B ,___ ,___ ,Key_Enter
   )
)
/* *INDENT-ON* */

KALEIDOSCOPE_INIT_PLUGINS(
  EEPROMSettings,
  EEPROMKeymap,
  Focus,
  FocusEEPROMCommand,
  FocusSettingsCommand,
  Qukeys,
  KeyTimings,
  Chords,
  OneShot,
  SpaceCadet,
  OneShot,
  Macros,
  MouseKeys
);

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {
  case MACRO_QWERTY:
    // This macro is currently unused, but is kept around for compatibility
    // reasons. We used to use it in place of `MoveToLayer(QWERTY)`, but no
    // longer do. We keep it so that if someone still has the old layout with
    // the macro in EEPROM, it will keep working after a firmware update.
    Layer.move(QWERTY);
    break;
  case MACRO_VERSION_INFO:
    if (keyToggledOn(keyState)) {
      Macros.type(PSTR("Keyboardio Atreus - Kaleidoscope "));
      Macros.type(PSTR(BUILD_INFORMATION));
    }
    break;
  default:
    break;
  }

  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.setup();
  SpaceCadet.disable();
  EEPROMKeymap.setup(10);
}

void loop() {
  Kaleidoscope.loop();
}
