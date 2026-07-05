#pragma once

#include <string>

// Stub module for the "Meeting notes and action extractor" tool entry.
// Project management.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace meeting_notes_and_action_extractor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
