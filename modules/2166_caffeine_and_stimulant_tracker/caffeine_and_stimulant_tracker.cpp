// SPDX-License-Identifier: GPL-3.0-or-later
#include "caffeine_and_stimulant_tracker.hpp"

namespace caffeine_and_stimulant_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Caffeine and Stimulant Tracker (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
