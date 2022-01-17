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
#include "Kaleidoscope-Steno.h"
#include "Kaleidoscope-SimpleChords.h"

enum {
  _QWERTY,
  _STENO,
};

/* *INDENT-OFF* */
KEYMAPS(

 [_QWERTY] = KEYMAP(
    Key_Tab,       Key_Q,    Key_W,    Key_E,    Key_R,    Key_T,    Key_Y,    Key_U,    Key_I,    Key_O,    Key_P,    Key_Backspace,
    Key_Escape,    Key_A,    Key_S,    Key_D,    Key_F,    Key_G,    Key_H,    Key_J,    Key_K,    Key_L,    Key_Semicolon, Key_Quote,
    Key_LeftShift, Key_Z,    Key_X,    Key_C,    Key_V,    Key_B,    Key_N,    Key_M,    Key_Comma, Key_Period,  Key_Slash, Key_Enter ,
                                                MoveToLayer(_STENO), Key_Space    , Key_LeftControl   , Key_LeftShift
  ),

/* Steno (GeminiPR)
 * ,-----------------------.      ,-----------------------.
 * | # | # | # | # | # | # |      | # | # | # | # | # | # |
 * |---+---+---+---+---+---|      |---+---+---+---+---+---|
 * |   | S | T | P | H | * |      | * | F | P | L | T | D |
 * |---+---+---+---+---+---|      |---+---+---+---+---+---|
 * |   | S | K | W | R | * |      | * | R | B | G | S | Z |
 * `-------------+---+---+-'      `-+---+---+-------------'
 *               | A | O |          | E | U |
 *               `-------'          `-------'
 */
 [_STENO] = KEYMAP(
    S(N1) ,S(N2) ,S(N3) ,S(N4) ,S(N5) ,S(N6)    ,S(N7)  ,S(N8) ,S(N9) ,S(NA) ,S(NB) ,S(NC)
   ,XXX   ,S(S1) ,S(TL) ,S(PL) ,S(HL) ,S(ST1)   ,S(ST3) ,S(FR) ,S(PR) ,S(LR) ,S(TR) ,S(DR)
   ,MoveToLayer(_QWERTY)   ,S(S2) ,S(KL) ,S(WL) ,S(RL) ,S(ST2)   ,S(ST4) ,S(RR) ,S(BR) ,S(GR) ,S(SR) ,S(ZR)
                               ,S(A)  ,S(O)     ,S(E)   ,S(U)
  )
);
/* *INDENT-ON* */

USE_SIMPLE_CHORDS(
  {
    .length = 2,
    .keys = {R1C4, R0C4},
    .action = Key_Equals
  }, {
    .length = 2,
    .keys = {R1C4, R2C4},
    .action = Key_Minus
  })

KALEIDOSCOPE_INIT_PLUGINS(Focus, GeminiPR, SimpleChords);

void setup() {
  Kaleidoscope.serialPort().begin(9600);
  Kaleidoscope.setup();
}

void loop() {
  Kaleidoscope.loop();
}
