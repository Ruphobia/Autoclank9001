#pragma once

#include <string>

// Stub module for the "Dream Journal with Motif Tagger" tool entry.
// Sleep science, wellness, mental health.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace dream_journal_with_motif_tagger {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
