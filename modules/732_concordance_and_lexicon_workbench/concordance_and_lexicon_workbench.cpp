#include "concordance_and_lexicon_workbench.hpp"

namespace concordance_and_lexicon_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Concordance and Lexicon Workbench (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
