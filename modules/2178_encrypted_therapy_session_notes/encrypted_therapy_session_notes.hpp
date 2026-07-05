#pragma once

#include <string>

// Stub module for the "Encrypted therapy session notes" tool entry.
// Sleep science, wellness, mental health.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace encrypted_therapy_session_notes {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
