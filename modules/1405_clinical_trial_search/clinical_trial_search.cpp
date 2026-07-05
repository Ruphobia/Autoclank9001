#include "clinical_trial_search.hpp"

namespace clinical_trial_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Clinical Trial Search (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
