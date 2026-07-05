#include "spaced_repetition_deck_builder.hpp"

namespace spaced_repetition_deck_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spaced Repetition Deck Builder (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
