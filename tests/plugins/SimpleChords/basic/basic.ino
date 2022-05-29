/* -*- mode: c++ -*-
 * Copyright (C) 2020  Keyboard.io, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Kaleidoscope.h>
#include <Kaleidoscope-SimpleChords.h>

#include "./common.h"

// *INDENT-OFF*
KEYMAPS(
    [0] = KEYMAP_STACKED
    (
        ___, ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___,
        ___,

        ___, ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___,
        ___
    ),
)
// *INDENT-ON*

USE_SIMPLE_CHORDS(
  {.length = 2, .keys = {R0C0, R0C1}, .action = Key_A},
  {.length = 3, .keys = {R0C0, R0C1, R0C2}, .action = Key_B},
);

KALEIDOSCOPE_INIT_PLUGINS(SimpleChords);

void setup() {
  Kaleidoscope.setup();
}

void loop() {
  Kaleidoscope.loop();
}