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

  // If inactive and a relevant key was pressed, eat it and switch to activating.
  if (state == INACTIVE) {
    if (keyToggledOn(key_state) && mapped_key == Key_C) {
      keys_pressed |= 1;
      state = ACTIVATING;
      last_time = now;
      ::Focus.send(F("Activating chord on C\n")); 
      return EventHandlerResult::EVENT_CONSUMED;
    }
    if (keyToggledOn(key_state) && mapped_key == Key_V) {
      keys_pressed |= 2;
      state = ACTIVATING;
      last_time = now;
      ::Focus.send(F("Activating chord on V\n")); 
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
  if (state == ACTIVATING) {
    EventHandlerResult ret = EventHandlerResult::OK;
    // Check for new relevant keys
    if (keyToggledOn(key_state) && mapped_key == Key_C) {
      keys_pressed |= 1;
      last_time = now;
      ret = EventHandlerResult::EVENT_CONSUMED;
    }
    if (keyToggledOn(key_state) && mapped_key == Key_V) {
      keys_pressed |= 2;
      last_time = now;
      ret = EventHandlerResult::EVENT_CONSUMED;
    }

    // Check if a relevant key was released
    if (keyToggledOff(key_state) && mapped_key == Key_C) {
      keys_pressed &= ~1;
      ret = EventHandlerResult::EVENT_CONSUMED;
      state = keys_pressed ? DERAMPING : INACTIVE;
      ::Focus.send(F("Pressing and releasing C on release\n")); 
      return EventHandlerResult::EVENT_CONSUMED;
    }
    if (keyToggledOff(key_state) && mapped_key == Key_V) {
      keys_pressed &= ~2;
      state = keys_pressed ? DERAMPING : INACTIVE;
      ret = EventHandlerResult::EVENT_CONSUMED;
      ::Focus.send(F("Pressing and releasing V on release\n")); 
      return EventHandlerResult::EVENT_CONSUMED;
    }


    // Check if we should activate the chord
    if (keys_pressed == 3) {
      ::Focus.send(F("Activating chord\n")); 
      state = ACTIVE;
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
      state = DERAMPING;
    }

    if(ongoing && (mapped_key == Key_C || mapped_key == Key_V))
        ret = EventHandlerResult::EVENT_CONSUMED;
    return ret;
  }

  // If a relevant key was released, release the chord and go to deactivating
  if (state == ACTIVE) {
    // Check if a relevant key was released
    if (keyToggledOff(key_state) && mapped_key == Key_C) {
      keys_pressed &= ~1;
      state = keys_pressed ? DEACTIVATING : INACTIVE;
      ::Focus.send(F("Releasing chord (on C)\n")); 
      mapped_key = Key_LeftShift;
      return EventHandlerResult::OK;
    }
    if (keyToggledOff(key_state) && mapped_key == Key_V) {
      keys_pressed &= ~2;
      state = keys_pressed ? DEACTIVATING : INACTIVE;
      ::Focus.send(F("Releasing chord (on V)\n")); 
      mapped_key = Key_LeftShift;
      return EventHandlerResult::OK;
    }

    // Pick on arbitrarily to pass through for chord still pressed; ignore the other.
    if(ongoing && (mapped_key == Key_C))
      return EventHandlerResult::EVENT_CONSUMED;
    if(ongoing && (mapped_key == Key_V)) {
      mapped_key = Key_LeftShift;
      return EventHandlerResult::OK;
    }
    return EventHandlerResult::OK;
  }

  if (state == DEACTIVATING) {
    EventHandlerResult ret = EventHandlerResult::OK;

    // Check if a relevant key was released
    if (keyToggledOff(key_state) && mapped_key == Key_C) {
      keys_pressed &= ~1;
      ret = EventHandlerResult::EVENT_CONSUMED;
      ::Focus.send(F("Releasing inactive chord key C\n")); 
    }
    if (keyToggledOff(key_state) && mapped_key == Key_V) {
      keys_pressed &= ~2;
      ret = EventHandlerResult::EVENT_CONSUMED;
      ::Focus.send(F("Releasing inactive chord key V\n")); 
    }

    if(keys_pressed == 0)
      state = INACTIVE;

    if(ongoing && (mapped_key == Key_C || mapped_key == Key_V))
      ret = EventHandlerResult::EVENT_CONSUMED;
    return ret;
  }

  if (state == DERAMPING) {
    // Check if a relevant key was released
    if (keyToggledOff(key_state) && mapped_key == Key_C) {
      keys_pressed &= ~1;
      ::Focus.send(F("Releasing deramping chord key C\n")); 
    }
    if (keyToggledOff(key_state) && mapped_key == Key_V) {
      keys_pressed &= ~2;
      ::Focus.send(F("Releasing deramping chord key V\n")); 
    }

    if(keys_pressed == 0)
      state = INACTIVE;

    return EventHandlerResult::OK;
  }

  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::Chords Chords;
