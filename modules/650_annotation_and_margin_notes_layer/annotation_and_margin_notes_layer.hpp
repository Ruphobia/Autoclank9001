#pragma once

#include <string>

// Stub module for the "Annotation and Margin-Notes Layer" tool entry.
// Academic and scholarly research.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace annotation_and_margin_notes_layer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
