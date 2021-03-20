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
#include "kaleidoscope/layers.h"

namespace kaleidoscope {
namespace plugin {

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
  } else if (strcmp_P(command + 11, PSTR("stop")) == 0){
    ::Focus.send(F("Key timing stopped!"));
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

}
}

kaleidoscope::plugin::KeyTimings KeyTimings;
