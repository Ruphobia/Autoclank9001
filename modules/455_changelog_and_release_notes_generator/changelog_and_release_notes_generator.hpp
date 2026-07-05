#pragma once

#include <string>

// Stub module for the "Changelog and release-notes generator" tool entry.
// Project management.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace changelog_and_release_notes_generator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
