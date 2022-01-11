/* -*- mode: c++ -*-
 * Kaleidoscope-SimpleChords -- SimpleChords
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

#pragma once

#include "kaleidoscope/KeyEventTracker.h"
#include "kaleidoscope/Runtime.h"

namespace kaleidoscope {
namespace plugin {
class SimpleChords : public kaleidoscope::Plugin {
 public:
  typedef struct {
    uint8_t length;
    Key keys[5];
    Key action;
  } Chord;

  SimpleChords(void) {}

  EventHandlerResult onSetup();
  EventHandlerResult afterEachCycle();
  EventHandlerResult onKeyswitchEvent(KeyEvent &);

 private:
  KeyEventTracker event_tracker_;

  int nqueued_events_;

  void expireEventAt(int);
  void queueEvent(KeyEvent &event);
  void clearQueue();
  void replayQueue();
  void sendChord(int index);
  void checkChords();
};
}
}

extern kaleidoscope::plugin::SimpleChords SimpleChords;
