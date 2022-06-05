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


#define USE_SIMPLE_CHORDS(...)                                            \
  namespace kaleidoscope {                                                \
  namespace plugin {                                                      \
  namespace simplechords {                                                \
  const kaleidoscope::plugin::SimpleChords::Chord chords[]         =      \
    {__VA_ARGS__};                                                        \
                                                                          \
  const uint8_t nchords = sizeof(chords) / sizeof(*chords);               \
  }                                                                       \
  }                                                                       \
  }

#define MAX_CHORD_LENGTH 5

namespace kaleidoscope {
namespace plugin {
class SimpleChords : public kaleidoscope::Plugin {
 public:
  typedef struct {
    KeyAddr keys[MAX_CHORD_LENGTH];
    Key action;
  } Chord;

  SimpleChords(void) {}

  EventHandlerResult onSetup();
  EventHandlerResult afterEachCycle();
  EventHandlerResult onKeyswitchEvent(KeyEvent &);

  static int16_t timeout_;
  static int32_t ignoreOnLayers_;

 private:
  KeyEventTracker event_tracker_;

  int nqueued_events_;

  void expireEventAt(int);
  void queueEvent(KeyEvent &event);
  void clearQueue();
  void replayQueue();
  void sendChord(int index);
  void releaseChord(int index);
  bool checkChords(bool sendSubset);
};

namespace simplechords {
  extern const SimpleChords::Chord chords[];
  extern const uint8_t nchords;
}
}
}

extern kaleidoscope::plugin::SimpleChords SimpleChords;
