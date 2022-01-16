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
#define BUILD_INFORMATION "locally built on " __DATE__ " at " __TIME__
#endif

#include "Kaleidoscope.h"
//#include "Kaleidoscope-EEPROM-Settings.h"
//#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-SimpleChords.h"
#include "Kaleidoscope-KeyTimings.h"
#include "Kaleidoscope-Macros.h"
//#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-OneShot.h"
//#include "Kaleidoscope-Qukeys.h"
//#include "Kaleidoscope-SpaceCadet.h"



#define MO(n) ShiftToLayer(n)
#define TG(n) LockLayer(n)
#define UN(n) UnlockLayer(n)

enum {
  MACRO_QWERTY,
  MACRO_VERSION_INFO,
  MACRO_ACCENT_GRAVE = 2
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
       Key_Exclamation ,Key_At           ,Key_UpArrow   ,Key_Home             ,Key_End
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
      ,M(MACRO_VERSION_INFO) ,Consumer_VolumeIncrement ,Consumer_Mute ,XXX            ,___ ,TG(SHORTCUT)
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

USE_SIMPLE_CHORDS(
  {
    .length = 2,
    .keys = {Key_F, Key_R},
    .action = Key_Tab
  }, {
    .length = 2,
    .keys = {Key_V, Key_F},
    .action = Key_Minus
  }, {
    .length = 2,
    .keys = {Key_Z, Key_X},
    .action = Key_LeftGui
  }, {
    .length = 2,
    .keys = {Key_X, Key_C},
    .action = Key_LeftAlt
  }, {
    .length = 2,
    .keys = {Key_D, Key_C},
    .action = Key_RightAlt
  }, {
    .length = 2,
    .keys = {Key_C, Key_V},
    .action = Key_LeftControl
  }, {
    .length = 2,
    .keys = {Key_Slash, Key_Period},
    .action = Key_LeftGui
  }, {
    .length = 2,
    .keys = {Key_Period, Key_Comma},
    .action = Key_LeftAlt
  }, {
    .length = 2,
    .keys = {Key_Comma, Key_M},
    .action = Key_LeftControl
  }, {
    .length = 2,
    .keys = {Key_K, Key_Comma},
    .action = Key_RightAlt
  }, {
    .length = 2,
    .keys = {Key_J, Key_M},
    .action = Key_Escape
  }, {
    .length = 2,
    .keys = {Key_Backtick, Key_B},
    .action = M(2)
  })

KALEIDOSCOPE_INIT_PLUGINS(
  //EEPROMSettings,
  //EEPROMKeymap,
  Focus,
  //FocusEEPROMCommand,
  //FocusSettingsCommand,
  //Qukeys,
  //KeyTimings,
  SimpleChords,
  //OneShot,
  Macros
  //MouseKeys
);

const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    switch (macro_id) {
    case MACRO_QWERTY:
      // This macro is currently unused, but is kept around for compatibility
      // reasons. We used to use it in place of `MoveToLayer(QWERTY)`, but no
      // longer do. We keep it so that if someone still has the old layout with
      // the macro in EEPROM, it will keep working after a firmware update.
      Layer.move(QWERTY);
      break;
    case MACRO_VERSION_INFO:
      Macros.type(PSTR("Keyboardio Atreus - Kaleidoscope "));
      Macros.type(PSTR(BUILD_INFORMATION));
      break;
    case MACRO_ACCENT_GRAVE:
      return MACRO(I(50), D(RightAlt), T(Backtick), U(RightAlt));
      break;
    default:
      break;
    }
  }
  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.setup();
  //SpaceCadet.disable();
  //EEPROMKeymap.setup(10);
}

void loop() {
  Kaleidoscope.loop();
}
