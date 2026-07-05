#include "lexicon_builder.hpp"

namespace lexicon_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lexicon Builder (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
