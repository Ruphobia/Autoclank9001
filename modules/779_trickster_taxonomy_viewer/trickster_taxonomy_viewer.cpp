#include "trickster_taxonomy_viewer.hpp"

namespace trickster_taxonomy_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Trickster Taxonomy Viewer (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
