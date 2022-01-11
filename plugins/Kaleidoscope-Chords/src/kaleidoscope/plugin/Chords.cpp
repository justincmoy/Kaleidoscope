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
 * TODO: handle overlapping chords.  This is not trivial.
 *
 *
 */

#include <Kaleidoscope-Chords.h>
#include <Kaleidoscope-FocusSerial.h>
#include "kaleidoscope/keyswitch_state.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/device/key_indexes.h"

#include "Kaleidoscope-Macros.h"

#define CHORD_TIMEOUT 24

namespace kaleidoscope {
namespace plugin {

Chords::Chord chords[] = {
  {
    .length = 2,
    .keys = {Key_F, Key_R},
    .action = Key_Tab
  }, /*{
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
  }*/};


const int nchords = sizeof(chords) / sizeof(chords[0]);


Chords::ChordState chordStates[nchords];

EventHandlerResult Chords::onSetup() {
  for(int i = 0; i < nchords; i++) {
    chordStates[i].state = Chords::INACTIVE;
    chordStates[i].pressed = 0;
    chordStates[i].last_time = 0;
  }

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

EventHandlerResult Chords::afterEachCycle() {
  for (int i = 0; i < nchords; i++) {
    Chord &chord = chords[i];
    ChordState &chordState = chordStates[i];

    if (chordState.state == PARTIAL && Runtime.hasTimeExpired(chordState.last_time, CHORD_TIMEOUT)) {
      // TODO: actually press the keys.
      ::Focus.send(F("Aborting chord on timeout 1:"), i, chordState.state, "\r\n");
      chordState.state = ABORTED;
      ::Focus.send(F("Aborting chord on timeout 2:"), i, chordState.state, "\r\n");
    }
  }

  return EventHandlerResult::OK;
}

Chords::KeyAction Chords::processChord(Chord &chord, ChordState &chordState, KeyEvent &event) {
  Key mapped_key = event.key;
  uint8_t key_state = event.state;
  uint32_t now = Runtime.millisAtCycleStart();

  bool ongoing = !keyToggledOn(key_state) && keyIsPressed(key_state);
  int8_t chordKeyIndex = -1;
  int i;

  for (i = 0; i < chord.length; i++)
    if (chord.keys[i] == mapped_key)
      chordKeyIndex = i;

  // Check for keys pressed or released, and update state appropriately.
  if (chordKeyIndex != -1 && keyToggledOn(key_state)) {
    chordState.last_time = now;
    chordState.pressed |= (1 << chordKeyIndex);
  }
  if (chordKeyIndex != -1 && keyToggledOff(key_state)) {
    chordState.pressed &= ~(1 << chordKeyIndex);
    //::Focus.send(F("Releasing key: <"));
    //::Focus.send(chordState.pressed);
    //::Focus.send(F(">\r\n"));
  }

  ::Focus.send(F("processing key, chordState.pressed is"), chordState.pressed, "and state is", chordState.state, "\n\r");
  // If inactive and a relevant key was pressed, eat it and switch to activating.
  if (chordState.state == INACTIVE) {
    if (chordState.pressed) {
      chordState.state = PARTIAL;
      ::Focus.send(F("Starting partial chord; eating key\n\r"));
      return CONSUME;
    }
    return IGNORE;
  }

  // If activating
  // - If another relevant key was pressed
  //   - if all keys are pressed: activate the chord, switch to activated.
  //   - If it's still partial, reset the timer
  // - If a relevant key was released, abort the chord
  // - If the timeout triggered, abort the chord
  // TODO: On deramping/deactivating, track re-presses to ensure chord is actually inactive when inactivated
  if (chordState.state == PARTIAL) {

    // If all keys are pressed, activate the chord
    if (chordState.pressed == (1 << chord.length) - 1) {
      ::Focus.send(F("Pressing chord; replacing key with chord\r\n"));
      chordState.state = PRESSED;
      //mapped_key = chord.action;
      return IGNORE;
    }

    // Timeout is handled in afterEachCycle.

    // Eat relevant key presses, and abort on relevant releases
    if (chordKeyIndex != -1) {
      if (keyToggledOff(key_state)) {
        chordState.state = chordState.pressed ? ABORTED : INACTIVE;
        // TODO: Press the key before releasing it
        ::Focus.send(F("Aborting chord on key release\r\n"));
      }
      //return CONSUME;
    }


    return IGNORE;
  }

  // If a relevant key was released, release the chord and go to deactivating
  if (chordState.state == PRESSED) {
    ::Focus.send(F("Chord is pressed\r\n"));
    // Check if a relevant key was released
    if (keyToggledOff(key_state) && chordState.pressed != (1 << chord.length) - 1) {
      chordState.state = chordState.pressed ? RELEASED : INACTIVE;
      ::Focus.send(F("Releasing chord\r\n"));
      mapped_key = chord.action;
      return IGNORE;
    }

    // Pick one arbitrarily to pass through for chord still pressed; ignore the other.
    if(ongoing && chordKeyIndex == 0) {
      mapped_key = chord.action;
      return IGNORE;
    }
    if(ongoing && chordKeyIndex != -1)
      return CONSUME;

    return IGNORE;
  }

  if (chordState.state == RELEASED) {
    ::Focus.send(F("Chord is released\r\n"));
    // Check if a relevant key was released
    if(chordState.pressed == 0)
      chordState.state = INACTIVE;

    // Check if the chord is fully pressed again
    if (chordState.pressed == (1 << chord.length) - 1) {
      ::Focus.send(F("Pressing chord\r\n"));
      chordState.state = PRESSED;
      //mapped_key = chord.action;
      return IGNORE;
    }

    // Check if a relevant key was released
    if (chordKeyIndex != -1)
      return CONSUME;

    return IGNORE;
  }

  if (chordState.state == ABORTED) {
    // Check if chord is fully released
    if(chordState.pressed == 0) {
      ::Focus.send(F("ABORTED switching back to INACTIVE\r\n"));
      chordState.state = INACTIVE;
    }

    return IGNORE;
  }

  return IGNORE;
}

EventHandlerResult Chords::onKeyswitchEvent(KeyEvent &event) {
  EventHandlerResult ret = EventHandlerResult::OK;
  int activate = -1;

  if (event_tracker_.shouldIgnore(event))
    return ret;

  for (int i = 0; i < nchords; i++) {
    if (processChord(chords[i], chordStates[i], event) == CONSUME)
      ret = EventHandlerResult::ABORT;
    // TODO: handle multiple chords activating simultaneously
    // Is this possible with event id's?
    if (processChord(chords[i], chordStates[i], event) == ACTIVATE)
      activate = i;
  }

  if (activate) {
    ::Focus.send(F("Activating chord"), activate, F("\r\n"));
  }

  return ret;
}

}
}

kaleidoscope::plugin::Chords Chords;
