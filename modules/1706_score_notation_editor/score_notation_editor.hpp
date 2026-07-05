#pragma once

#include <string>

// Stub module for the "Score Notation Editor" tool entry.
// Music composition and theory.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace score_notation_editor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
