#pragma once

#include <string>

// Stub module for the "Set Drafting Workspace" tool entry.
// Performing arts: theater, sound, lighting, stage.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace set_drafting_workspace {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
