#include "abbreviation_and_ligature_expander.hpp"

namespace abbreviation_and_ligature_expander {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Abbreviation and Ligature Expander (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
