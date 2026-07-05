#include "lexicon_lookup.hpp"

namespace lexicon_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lexicon lookup (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
