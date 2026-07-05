#pragma once

#include <string>

// Stub module for the "Trickster Taxonomy Viewer" tool entry.
// Mythology, folklore, and comparative narrative.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace trickster_taxonomy_viewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
