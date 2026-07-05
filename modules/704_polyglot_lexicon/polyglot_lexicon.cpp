#include "polyglot_lexicon.hpp"

namespace polyglot_lexicon {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Polyglot Lexicon (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
