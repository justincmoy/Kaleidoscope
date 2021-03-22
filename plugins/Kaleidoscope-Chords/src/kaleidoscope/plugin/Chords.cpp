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
 * State diagram:
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

static uint32_t last_time;
static uint8_t state;


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

static uint8_t keys_pressed = 0;

EventHandlerResult Chords::onKeyswitchEvent(Key &mapped_key, KeyAddr key_addr, uint8_t key_state) {
  uint32_t now = Runtime.millisAtCycleStart();

  bool ongoing = !keyToggledOn(key_state) && keyIsPressed(key_state);

  // Check for keys pressed or released, and update state appropriately.
  if (keyToggledOn(key_state) && mapped_key == Key_C) {
    last_time = now;
    keys_pressed |= 1;
  }
  if (keyToggledOn(key_state) && mapped_key == Key_V) {
    last_time = now;
    keys_pressed |= 2;
  }

  if (keyToggledOff(key_state) && mapped_key == Key_C)
    keys_pressed &= ~1;
  if (keyToggledOff(key_state) && mapped_key == Key_V)
    keys_pressed &= ~2;

  // If inactive and a relevant key was pressed, eat it and switch to activating.
  if (state == INACTIVE) {
    if (keys_pressed) {
      state = PARTIAL;
      ::Focus.send(F("Activating chord\n")); 
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
  if (state == PARTIAL) {

    // If all keys are pressed, activate the chord
    if (keys_pressed == 3) {
      ::Focus.send(F("Activating chord\n")); 
      state = PRESSED;
      mapped_key = Key_LeftShift;
      return EventHandlerResult::OK;
    }

    // Handle timeout
    // TODO: if two of three were pressed, these should be ordered.
    if (now - last_time > CHORD_TIMEOUT) {
      if(keys_pressed & 1)
        ::Focus.send(F("Pressing C on timeout\n")); 
      if(keys_pressed & 2)
        ::Focus.send(F("Pressing V on timeout\n")); 
      state = ABORTED;
    }

    // Eat relevant key presses, and abort on relevant releases
    if (mapped_key == Key_C || mapped_key == Key_V) {
      if (keyToggledOff(key_state)) {
        state = keys_pressed ? ABORTED : INACTIVE;
        // TODO: Press the key before releasing it
        ::Focus.send(F("Aborting chord on key release\n")); 
      }
      return EventHandlerResult::EVENT_CONSUMED;
    }


    return EventHandlerResult::EVENT_CONSUMED;
  }

  // If a relevant key was released, release the chord and go to deactivating
  if (state == PRESSED) {
    // Check if a relevant key was released
    if (keyToggledOff(key_state) && keys_pressed != 3) {
      state = keys_pressed ? RELEASED : INACTIVE;
      ::Focus.send(F("Releasing chord\n")); 
      mapped_key = Key_LeftShift;
      return EventHandlerResult::OK;
    }

    // Pick one arbitrarily to pass through for chord still pressed; ignore the other.
    if(ongoing && (mapped_key == Key_C))
      return EventHandlerResult::EVENT_CONSUMED;
    if(ongoing && (mapped_key == Key_V)) {
      mapped_key = Key_LeftShift;
      return EventHandlerResult::OK;
    }

    return EventHandlerResult::OK;
  }

  if (state == RELEASED) {
    if(keys_pressed == 0)
      state = INACTIVE;

    // Check if a relevant key was released
    if (mapped_key == Key_C || mapped_key == Key_V) {
      return EventHandlerResult::EVENT_CONSUMED;
    }

    return EventHandlerResult::OK;
  }

  if (state == ABORTED) {
    // Check if chord is fully released
    if(keys_pressed == 0)
      state = INACTIVE;

    return EventHandlerResult::OK;
  }

  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::Chords Chords;
