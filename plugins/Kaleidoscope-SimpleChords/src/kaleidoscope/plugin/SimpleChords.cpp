/* -*- mode: c++ -*-
 * Kaleidoscope-SimpleChords -- Simple chording support
 * Copyright (C) 2022  Evan Danaher
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
 * This implements a simple (but imperfect) notion of "chording", where mutiple keys can be quickly pressed to
 * result in a different action being taken.  This is similar to MagicCombo, but does not also send the
 * individual keypresses; this makes it useful for applying to non-modifier keys.  (E.g., mapping A and S to
 * escape in MagicCombo would result in AS<escape>, while here, it would result in simply <escape>).
 *
 * This is a simplified model in which all keys in the chord must be pressed not only within the timeout, but
 * also consecutively with no other keys in between.  (E.g., if "AS" is a chord, pressing "ADS" would not
 * activate the chord, regardless of timing).  Further, events remain strictly ordered: if ASD is a chord, and
 * AS is pressed and then S released, A-down, S-down, S-up would be sent.
 *
 * Further, as soon as a chord is detected, it is activated, and those keys cannot be used in other chords.
 * (For example, if AS, AD and ASD are all defined chords, pressing ASD will trigger AS, consume them, and
 * then press D, while ADS would similarly trigger AD and then press S, while SDA will produce only ASD, since
 * it is the only chord to fully match at any point.
 *
 * A chord is considered "released" as soon as one of its keys is released; once released, the key will be
 * processed as normal, but remaining keys held in the chord will be ignored until they are released.  (E.g.,
 * if you press a chord ASD and release D, you can press D again to send a D, but the keyboard will act as
 * though A and S are unpressed, and releasing them will have no impact, aside from allowing them to be
 * pressed again.)
 *
 * So avoid overlapping chords that may be pressed simultaneously, though cases such as QA and AZ are likely
 * fine, as it's unlikely that both would be pressed simultaneously.
 *
 * Note also that if the timeout is zero (no timeout), keys in chords will not be sent until they are released
 * or a non-chord key is pressed.  This is similar to, e.g., the Twiddler3, and may be useful in certain
 * special cases.
 *
 * Implementation:
 *
 * Intuitively, on a keypress, if it's part of a chord, delay it until either:
 *
 *   - a timeout is reached (send the keypress)
 *   - the chord is completed (replace the first key with the chord, and drop following keypresses)
 *   - a key that is not part of the chord is pressed (send the keypress, followed by the other key)
 *   - the key is released (send the keypress, followed by the release)
 *
 * Since a key can be part of multiple chords, chords can contain multiple keys, and keys can be released in
 * any order, this is implemented by tracking a queue of keys that can potentially be part of a chord.
 *
 * So more precisely:
 *
 * - On a keypress, we add the key and its timestamp to the end of the queue, and check for cases:
 *   - If the queue matches a chord, we send the chord and empty the queue.
 *   - If the queue is not a subset of any chord, remove the first key in the queue, send it, and repeat.
 * - On a keyrelease, we remove the key from the queue, along with any keys before it (to preserve ordering)
 *   - If the queue now matches a chord, we send the chord and empty the queue.
 *     (E.g., if ASDF is a chord, and so is AS, pressing DAS and then releasing D would trigger this.  This
 *     allows chords to be pressed immediately after another key, regardless of confounding chords.)
 *   - The queue was previously a subset of a chord, so it still is.
 * - If the first key in the queue times out, we treat it as if that key was released, sending it and checking
 *   the remainder of the queue for chords (though not not sending a key release event).
 *
 *
 * Note that this is somewhat limiting - most cases are probably handled, but the TextBlade uses ZX, XC, and
 * CV as modifiers - if ZX is bound to control, and XC to shift, pressing ZXC should result in both control
 * and shift being pressed, but that is not possible with this plugin.
 *
 * However, handling that is significantly more complex, and this likely handles most cases.  Hopefully a
 * future non-simple chords plugin will allow for that.
 *
 * Artificial limitations:
 * - A chord can have at most five keys in it
 * - If more than ten keys are pressed simultaneously, the oldest ones will be treated as timed out regardless
 *   of timing.
 */

#include <Kaleidoscope-SimpleChords.h>
#include <Kaleidoscope-FocusSerial.h>
#include "kaleidoscope/keyswitch_state.h"
#include "kaleidoscope/layers.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/device/key_indexes.h"

#include "Kaleidoscope-Macros.h"

#define QUEUE_LEN 10

#define DEBUG(...)
//#define DEBUG(...) ::Focus.send(__VA_ARGS__);




namespace kaleidoscope {
namespace plugin {

using namespace simplechords;

// Configuration
int16_t SimpleChords::timeout_ = 24;
int32_t SimpleChords::ignoreOnLayers_ = 0;


typedef struct {
  KeyEvent event;
  int time;
} queueItem;

queueItem queued_events_[QUEUE_LEN];

typedef struct {
  int index;
  KeyAddr addr;
} activeChord;

typedef struct {
  KeyAddr addr;
  int chord;
} consumedKey;

int nconsumed_keys_;
consumedKey consumed_keys_[QUEUE_LEN];
int nactive_chords;
activeChord active_chords_[QUEUE_LEN];

EventHandlerResult SimpleChords::onSetup() {
  nqueued_events_ = 0;

  return EventHandlerResult::OK;
}

void SimpleChords::expireEventAt(int index) {
    DEBUG(F("Removing and sending queued key"), queued_events_[index].event.key, "at index", index, "\r\n");
    Runtime.handleKeyswitchEvent(queued_events_[index].event);
    for (int i = index; i < nqueued_events_; i++)
      queued_events_[i] = queued_events_[i + 1];
    nqueued_events_--;
}

void SimpleChords::queueEvent(KeyEvent &event) {
  if (nqueued_events_ == QUEUE_LEN) {
    // TODO: Drop the first element from the queue instead of this one
    return;
  }
  DEBUG(F("Queueing key"), event.key, "/", event.addr.row(), ",", event.addr.col(), "at index", nqueued_events_, "\r\n");
  queued_events_[nqueued_events_].event = event;
  queued_events_[nqueued_events_].time = Runtime.millisAtCycleStart();
  nqueued_events_++;
}

void SimpleChords::clearQueue() {
  DEBUG(F("Clearing queuer\r\n"));
  nqueued_events_ = 0;
}

void SimpleChords::replayQueue() {
  DEBUG(F("Replaying queue\r\n"));
  for (int i = 0; i < nqueued_events_; i++) {
    DEBUG(F("Sending queued event"), queued_events_[i].event.key, "at index", i, "\r\n");
    Runtime.handleKeyswitchEvent(queued_events_[i].event);
  }
  clearQueue();
}

void SimpleChords::sendChord(int index) {
  DEBUG(F("Sending chord"), index, "\r\n");
  // Arbitrarily pick the first event as the one to send with modified key
  queued_events_[0].event.key = chords[index].action;
  Runtime.handleKeyswitchEvent(queued_events_[0].event);
  // NOTE: if we're sending a chord, it's keys are the first <length> keys in the queue.
  for (int i = 0; i < chords[index].length; i++) {
    consumed_keys_[nconsumed_keys_].addr = queued_events_[i].event.addr;
    consumed_keys_[nconsumed_keys_].chord = index;
    nconsumed_keys_++;
  }
  active_chords_[nactive_chords].index = index;
  active_chords_[nactive_chords].addr = queued_events_[0].event.addr;
  nactive_chords++;

  // Remove the chord events from the queue.
  nqueued_events_ -= chords[index].length;
  for (int i = 0; i < nqueued_events_; i++)
    queued_events_[i] = queued_events_[i + chords[index].length];
}

void SimpleChords::releaseChord(int active_index) {
  int index = active_chords_[active_index].index;
  int i;
  KeyEvent event = KeyEvent(active_chords_[active_index].addr, WAS_PRESSED, chords[index].action);

  DEBUG(F("Releasing chord"), index, "\r\n");
  Runtime.handleKeyswitchEvent(event);

  for (i = 0; i < nactive_chords && active_chords_[i].index != index; i++);
  nactive_chords--;
  for (; i < nactive_chords; i++)
    active_chords_[i] = active_chords_[i + 1];
}

void SimpleChords::checkChords() {
  int c, i, j;
  bool partial_match;
  

  DEBUG(F("Checking chords\r\n"));
  while (nqueued_events_ > 0) {
    partial_match = false;
    for (c = 0; c < nchords; c++) {
      // A shorter chord can't match
      if (chords[c].length < nqueued_events_)
        continue;
      for (i = 0; i < nqueued_events_; i++) {
        for (j = 0; j < chords[c].length; j++)
          if(queued_events_[i].event.addr.toInt() + 1 == chords[c].keys[j])
            break;
        // The key wasn't found in the queue; abort this chord.
        if (j == chords[c].length)
          break;
      }

      // No key wasn't found, so the queue is a subset of the chord!
      if (i == nqueued_events_) {
        DEBUG(F("Found a subset of chord"), c, "\r\n");
        // if the queue is the length of the chord, we matched!  Send it!
        if (chords[c].length == nqueued_events_) {
          sendChord(c);
          clearQueue();
          // Only one chord can match, so bail early.
          return;
        } else {
          partial_match = true;
        }
      }
    }
    if (!partial_match) {
      DEBUG(F("No partial chords; expiring a character\r\n"));
      expireEventAt(0);
    } else {
      DEBUG(F("partial match found; done checking chords\r\n"));
      break;
    }
  }
}

EventHandlerResult SimpleChords::afterEachCycle() {
  while (nqueued_events_ > 0 && Runtime.hasTimeExpired(queued_events_[0].time, timeout_)) {
    DEBUG(F("Removing expired event\r\n"));
    expireEventAt(0);
    checkChords();
  }

  return EventHandlerResult::OK;
}

EventHandlerResult SimpleChords::onKeyswitchEvent(KeyEvent &event) {
  const uint8_t key_state = event.state;

  uint8_t i, j, k;

  DEBUG("#### Get key addr", event.addr.toInt(), "vs first chord ", chords[0].keys[0], chords[0].keys[1], "\r\n", R0C0, R0C1, R0C2, R0C3, " - ", R1C0, R2C0, R3C0, R4C0);

  int32_t layers = ignoreOnLayers_;
  for (i = 0; layers; i++) {
    if (layers & 1 && Layer.isActive(i))
      return EventHandlerResult::OK;
    layers >>= 1;
  }

  if (event_tracker_.shouldIgnore(event))
    return EventHandlerResult::OK;

  if (keyIsPressed(event.state)) {
    // Check if the key is in any chords
    // Note: This could be combined with the checking for activated chords, but KISS.
    for (i = 0; i < nchords; i++) {
      for (j = 0; j < chords[i].length; j++)
        if (chords[i].keys[j] == event.addr.toInt() + 1) {
          break;
        }
      // If new key is in the chord, break
      if (j != chords[i].length) {
        DEBUG(F("Found chord match at"), i, "\r\n");
        break;
      }
    }

    if(i != nchords) {
      // The key is in a chord, so add it to the queue and check if we have a chord!
      queueEvent(event);
      checkChords();
      return EventHandlerResult::ABORT;
    } else  {
      // The key is in not in a chord, so break the chord.
      replayQueue();
      return EventHandlerResult::OK;
    }
  } else  {
    DEBUG(F("Releasing key"), event.key, "/", event.addr.row(), ",", event.addr.col(), "\r\n");
    for (i = 0; i < nqueued_events_; i++)
      if (event.addr == queued_events_[i].event.addr)
        break;

    // If it's in the queue, send it.
    // TODO: This should clear out the queue before it.
    if (i != nqueued_events_) {
      DEBUG(F(" Sending queued event for released key"), queued_events_[i].event.key, "at index", i, "\r\n");
      expireEventAt(i);
    }

    if (nconsumed_keys_ > 0) {
      DEBUG(F(" Checking against "), nconsumed_keys_, "consumed keys\r\n");
    }
    for (i = 0; i < nconsumed_keys_; i++)
      DEBUG(F("  Key: "), consumed_keys_[i].addr.row(), ",", consumed_keys_[i].addr.col(), "\r\n");

    for (k = 0; k < nconsumed_keys_; k++)
      if (event.addr == consumed_keys_[k].addr)
        break;

    if (k != nconsumed_keys_) {
      DEBUG(F(" Eating consumed event for released key"), event.addr.row(), ",", event.addr.col(), "at index", i, "of", nconsumed_keys_, "\r\n");
      // Check if the chord this key was part of is still active.
      for (i = 0; i < nactive_chords; i++) {
        int c = active_chords_[i].index;
        if (c == consumed_keys_[k].chord)
          releaseChord(i);
      }

      // The key release has been consumed; remove it from the list.
      nconsumed_keys_--;
      for (; k < nconsumed_keys_; k++)
        consumed_keys_[k] = consumed_keys_[k+1];


      return EventHandlerResult::ABORT;
    }
  }

  return EventHandlerResult::OK;
}

}
}


kaleidoscope::plugin::SimpleChords SimpleChords;
