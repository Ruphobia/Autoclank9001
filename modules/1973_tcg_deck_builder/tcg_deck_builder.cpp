#include "tcg_deck_builder.hpp"

namespace tcg_deck_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TCG deck builder (Game playing assistants). Awaits wire-up.";
    return s;
}

}
