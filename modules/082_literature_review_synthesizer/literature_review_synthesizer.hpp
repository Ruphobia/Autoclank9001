#pragma once

#include <string>

// Stub module for the "Literature Review Synthesizer" tool entry.
// Memory and knowledge accrual.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace literature_review_synthesizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
