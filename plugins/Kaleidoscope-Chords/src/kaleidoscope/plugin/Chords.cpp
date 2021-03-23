/* -*- mode: c++ -*-
 * Kaleidoscope-EEPROM-Keymap -- EEPROM-based keymap support.
 * Copyright (C) 2017, 2018, 2019  Keyboard.io, Inc
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

/*
 * States
 *
 * INACTIVE:
 *   No keys in the chord are pressed.
 *   - Pressing any key in the chord will change to PARTIAL
 *
 * PARTIAL:
 *   One or more (but not all) keys in the chord are pressed.
 *   The chord could be completed if all keys are pressed within the timeout.
 *   - Pressing another key in the chord resets the timer
 *   - Pressing the final key in the chord changes to PRESSED
 *   - Releasing any key in the chord chord (ideally) presses all pressed keys and changes to ABORTED
 *     - This is currently unimplemented, since it's basically impossible to do in the timeout
 *   - Timing out (ideally) presses all pressed keys and changes to ABORTED
 *     - Currently we just stop eating events, and that seems to work.
 *
 * PRESSED:
 *   The chord has been fully pressed in the timeout, and they key down event for the chorded key is sent.
 *   - Releasing any key in the chord releases the chorded key and changes to RELEASED
 *
 * RELEASED:
 *   The chord was pressed, and has been released.
 *   - Releasing any key in the chord will be eaten, since they were never pressed
 *   - Releasing all keys in the chord changes to INACTIVE
 *   - Pressing a key in the chord - currently unknown behavior.
 *
 * ABORTED:
 *   The chord was aborted, either via releasing a key or timeout.
 *   We pretend it never happened.
 *   - Releasing all keys in the chord changes to INACTIVE.
 *
 *
 * * -- C -->  C --- V ---> chord down ---- any key up --> chord up --- all keys up --> *
 * ^           |
 * |          15 ms
 * |           |
 * \           v
 *  \        send C
 *   \         |
 *    '------ all keys up
 *
 *
 *
 */

#include <Kaleidoscope-Chords.h>
#include <Kaleidoscope-FocusSerial.h>
#include "kaleidoscope/keyswitch_state.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/device/key_indexes.h"

#define CHORD_TIMEOUT 12

namespace kaleidoscope {
namespace plugin {

Chords::Chord chord = {
  .length = 2,
  .keys = {Key_A, Key_S},
  .action = Key_Tab
};

EventHandlerResult Chords::onSetup() {
  return EventHandlerResult::OK;
}

EventHandlerResult Chords::onFocusEvent(const char *command) {
  //if (::Focus.handleHelp(command, PSTR("keytimings.start\nkeytimings.stop")))
    return EventHandlerResult::OK;

  /*if (strncmp_P(command, PSTR("keytimings."), 11) != 0)
    return EventHandlerResult::OK;

  if (strcmp_P(command + 11, PSTR("start")) == 0) {
    ::Focus.send(F("Key timing started!"));
    tracking = 1;
  } else if (strcmp_P(command + 11, PSTR("stop")) == 0){
    ::Focus.send(F("Key timing stopped!"));
    tracking = 0;
  }*/

  //return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult Chords::onKeyswitchEvent(Key &mapped_key, KeyAddr key_addr, uint8_t key_state) {
  uint32_t now = Runtime.millisAtCycleStart();
  bool ongoing = !keyToggledOn(key_state) && keyIsPressed(key_state);
  int8_t chordKeyIndex = -1;
  int i;

  for (i = 0; i < chord.length; i++)
    if (chord.keys[i] == mapped_key)
      chordKeyIndex = i;

  // Check for keys pressed or released, and update state appropriately.
  if (chordKeyIndex != -1 && keyToggledOn(key_state)) {
    chord.last_time = now;
    chord.pressed |= (1 << chordKeyIndex);
  }
  if (chordKeyIndex != -1 && keyToggledOff(key_state)) {
    chord.pressed &= ~(1 << chordKeyIndex);
    ::Focus.send(F("Releasing key: <"));
    ::Focus.send(chord.pressed);
    ::Focus.send(F(">\n"));
  }

  // If inactive and a relevant key was pressed, eat it and switch to activating.
  if (chord.state == INACTIVE) {
    if (chord.pressed) {
      chord.state = PARTIAL;
      ::Focus.send(F("Starting partial chord\n"));
      return EventHandlerResult::EVENT_CONSUMED;
    }
    return EventHandlerResult::OK;
  }

  // If activating:
  // - If another relevant key was pressed
  //   - if all keys are pressed: activate the chord, switch to activated.
  //   - If it's still partial, reset the timer
  // - If a relevant key was released, abort the chord
  // - If the timeout triggered, abort the chord
  // TODO: On deramping/deactivating, track re-presses to ensure chord is actually inactive when inactivated
  if (chord.state == PARTIAL) {

    // If all keys are pressed, activate the chord
    if (chord.pressed == (1 << chord.length) - 1) {
      ::Focus.send(F("Pressing chord\n"));
      chord.state = PRESSED;
      mapped_key = chord.action;
      return EventHandlerResult::OK;
    }

    // Handle timeout
    // TODO: if two of three were pressed, these should be ordered.
    if (now - chord.last_time > CHORD_TIMEOUT) {
      // TODO: actually press the keys.
      // It turns out that just leaving them pressed works.
      ::Focus.send(F("Aborting chord on timeout\n"));
      chord.state = ABORTED;
    }

    // Eat relevant key presses, and abort on relevant releases
    if (chordKeyIndex != -1) {
      if (keyToggledOff(key_state)) {
        chord.state = chord.pressed ? ABORTED : INACTIVE;
        // TODO: Press the key before releasing it
        ::Focus.send(F("Aborting chord on key release\n"));
      }
      return EventHandlerResult::EVENT_CONSUMED;
    }


    return EventHandlerResult::OK;
  }

  // If a relevant key was released, release the chord and go to deactivating
  if (chord.state == PRESSED) {
    // Check if a relevant key was released
    if (keyToggledOff(key_state) && chord.pressed != (1 << chord.length) - 1) {
      chord.state = chord.pressed ? RELEASED : INACTIVE;
      ::Focus.send(F("Releasing chord\n"));
      mapped_key = chord.action;
      return EventHandlerResult::OK;
    }

    // Pick one arbitrarily to pass through for chord still pressed; ignore the other.
    if(ongoing && chordKeyIndex == 0) {
      mapped_key = chord.action;
      return EventHandlerResult::OK;
    }
    if(ongoing && chordKeyIndex != -1)
      return EventHandlerResult::EVENT_CONSUMED;

    return EventHandlerResult::OK;
  }

  if (chord.state == RELEASED) {
    if(chord.pressed == 0)
      chord.state = INACTIVE;

    // Check if a relevant key was released
    if (chordKeyIndex != -1)
      return EventHandlerResult::EVENT_CONSUMED;

    return EventHandlerResult::OK;
  }

  if (chord.state == ABORTED) {
    // Check if chord is fully released
    if(chord.pressed == 0)
      chord.state = INACTIVE;

    return EventHandlerResult::OK;
  }

  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::Chords Chords;
