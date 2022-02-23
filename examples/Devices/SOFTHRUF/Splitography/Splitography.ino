/* -*- mode: c++ -*-
 * Splitography -- A very basic Kaleidoscope example for the SOFT/HRUF Splitography keyboard
 * Copyright (C) 2018  Keyboard.io, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTabILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Kaleidoscope.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-Steno.h"
#include "Kaleidoscope-SimpleChords.h"

#define MO(n) ShiftToLayer(n)
#define TG(n) LockLayer(n)
#define UN(n) UnlockLayer(n)
#define ST(n) MoveToLayer(n)

enum {
  MACRO_ACCENT_GRAVE,
};

enum {
  _QWERTY,
  _FUN,
  _UPPER,
  _STENO
};

/* *INDENT-OFF* */
KEYMAPS(

 [_QWERTY] = KEYMAP_STACKED(
    Key_Tab,    Key_Q,    Key_W,    Key_E,    Key_R,    Key_T,    
    Key_LeftGui,   Key_A,    Key_S,    Key_D,    Key_F,    Key_G,    
    Key_LeftAlt,   Key_Z,    Key_X,    Key_C,    Key_V,    Key_B,    
                                              MO(_FUN), Key_Space,

    Key_Y,    Key_U,    Key_I,    Key_O,    Key_P,         Key_Backspace,
    Key_H,    Key_J,    Key_K,    Key_L,    Key_Semicolon, Key_Enter,
    Key_N,    Key_M,    Key_Comma, Key_Period,  Key_Slash, Key_Escape,
    Key_LeftShift   , Key_LeftControl


  ),

 [_FUN] = KEYMAP_STACKED (
    MoveToLayer(_UPPER),  LSHIFT(Key_1),   LSHIFT(Key_2),    Key_UpArrow,    Key_Home,               Key_End,
    Key_Escape,           Key_LeftParen,   Key_LeftArrow,    Key_DownArrow,  Key_RightArrow,         Key_RightParen,
    Key_LeftAlt,          Key_LeftBracket, Key_RightBracket, LSHIFT(Key_3),  Key_LeftCurlyBracket,   Key_RightCurlyBracket,
                                                MO(_FUN), Key_Space,


    Key_PageUp,     Key_7,  Key_8, Key_9, Key_Backtick,  Key_Delete,
    Key_PageDown,   Key_4,  Key_5, Key_6, Key_Quote,     Key_Minus,
    LSHIFT(Key_8),  Key_1,  Key_2, Key_3, Key_Backslash, Key_Equals,
    ___,            ___
  ),

 [_UPPER] = KEYMAP_STACKED (
    MoveToLayer(_STENO),  Key_Insert,   Key_Home,    Key_UpArrow,    Key_Home,               Key_PageUp,
    Key_Escape,           Key_Delete,   Key_LeftArrow,    Key_DownArrow,  Key_RightArrow,         Key_PageDown,
    Key_LeftAlt,          Consumer_VolumeDecrement, Consumer_VolumeIncrement, Consumer_Mute,  XXX,   XXX,
                                                MO(_FUN), MoveToLayer(_QWERTY),


    Key_UpArrow,     Key_F7,  Key_F8, Key_F9, Key_F10,  Key_PrintScreen,
    Key_DownArrow,   Key_F4,  Key_F5, Key_F6, Key_F11,  Key_ScrollLock,
    LSHIFT(Key_8),   Key_F1,  Key_F2, Key_F3, Key_F12, Consumer_PlaySlashPause,
    ___,            ___
  ),


/* Steno (GeminiPR)
 * ,-----------------------.      ,-----------------------.
 * | # | # | # | # | # | # |      | # | # | # | # | # | # |
 * |---+---+---+---+---+---|      |---+---+---+---+---+---|
 * |   | S | T | P | H | * |      | * | F | P | L | T | D |
 * |---+---+---+---+---+---|      |---+---+---+---+---+---|
 * |qwe| S | K | W | R | * |      | * | R | B | G | S | Z |
 * `-------------+---+---+-'      `-+---+---+-------------'
 *               | A | O |          | E | U |
 *               `-------'          `-------'
 */
 [_STENO] = KEYMAP(
    S(N1)               ,S(N2) ,S(N3) ,S(N4) ,S(N5) ,S(N6)    ,S(N7)  ,S(N8) ,S(N9) ,S(NA) ,S(NB) ,S(NC)
   ,Key_B                 ,S(S1) ,S(TL) ,S(PL) ,S(HL) ,S(ST1)   ,S(ST3) ,S(FR) ,S(PR) ,S(LR) ,S(TR) ,S(DR)
   ,ST(_QWERTY) ,S(S2) ,S(KL) ,S(WL) ,S(RL) ,S(ST2)   ,S(ST4) ,S(RR) ,S(BR) ,S(GR) ,S(SR) ,S(ZR)
                                              ,S(A)  ,S(O)     ,S(E)   ,S(U)
  )
);
/* *INDENT-ON* */

USE_SIMPLE_CHORDS(
  {
    .length = 2,
    .keys = {R1C4, R0C4},
    .action = Key_Tab
  }, {
    .length = 2,
    .keys = {R1C4, R2C4},
    .action = Key_Minus
  }, {
    .length = 2,
    .keys = {R1C3, R2C3},
    .action = Key_RightAlt
  }, {
    .length = 2,
    .keys = {R1C8, R2C8},
    .action = Key_RightAlt
  }, {
    .length = 2,
    .keys = {R1C7, R2C7},
    .action = Key_Escape
  }, {
    .length = 2,
    .keys = {R2C4, R2C5},
    .action = M(MACRO_ACCENT_GRAVE)
  })

KALEIDOSCOPE_INIT_PLUGINS(Focus, GeminiPR, Macros, SimpleChords);

const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    switch (macro_id) {
    case MACRO_ACCENT_GRAVE:
      return MACRO(D(RightAlt), T(Backtick), U(RightAlt));
      break;
    default:
      break;
    }
  }
  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.serialPort().begin(9600);
  Kaleidoscope.setup();
}

void loop() {
  Kaleidoscope.loop();
}
