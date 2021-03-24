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

#include <Kaleidoscope-KeyTimings.h>
#include <Kaleidoscope-FocusSerial.h>
#include "kaleidoscope/keyswitch_state.h"
#include "kaleidoscope/Runtime.h"

namespace kaleidoscope {
namespace plugin {

static uint32_t last_time;
static bool last_down;
static bool tracking;

EventHandlerResult KeyTimings::onSetup() {
  return EventHandlerResult::OK;
}

EventHandlerResult KeyTimings::onFocusEvent(const char *command) {
  if (::Focus.handleHelp(command, PSTR("keytimings.start\nkeytimings.stop")))
    return EventHandlerResult::OK;

  if (strncmp_P(command, PSTR("keytimings."), 11) != 0)
    return EventHandlerResult::OK;

  if (strcmp_P(command + 11, PSTR("start")) == 0) {
    ::Focus.send(F("Key timing started!"));
    tracking = 1;
  } else if (strcmp_P(command + 11, PSTR("stop")) == 0){
    ::Focus.send(F("Key timing stopped!"));
    tracking = 0;
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult KeyTimings::onKeyswitchEvent(Key &mapped_key, KeyAddr key_addr, uint8_t key_state) {
  uint32_t now = Runtime.millisAtCycleStart();

  if(!tracking)
    return EventHandlerResult::OK;

  if (keyToggledOn(key_state)) {
    ::Focus.send(now - last_time);
    ::Focus.send(F("Key pressed")); 
    ::Focus.send(mapped_key); 
    if(last_down && now - last_time < 15)
      ::Focus.send(F("chord")); 
    ::Focus.send(F("\n")); 
    last_time = now;
    last_down = true;
  }
  else if (keyToggledOff(key_state)) {
    //::Focus.send(now - last_time);
    //::Focus.send(F("Key released\n"));
    //last_time = now;
    last_down = false;
  }

  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::KeyTimings KeyTimings;
