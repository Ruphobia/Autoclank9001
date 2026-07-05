#include "corpus_concordancer.hpp"

namespace corpus_concordancer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Corpus Concordancer (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
