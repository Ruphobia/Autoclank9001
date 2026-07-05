// SPDX-License-Identifier: GPL-3.0-or-later
#include "meeting_notes_and_action_extractor.hpp"

namespace meeting_notes_and_action_extractor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Meeting notes and action extractor (Project management). Awaits wire-up.";
    return s;
}

}
