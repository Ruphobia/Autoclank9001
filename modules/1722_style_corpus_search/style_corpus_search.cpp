#include "style_corpus_search.hpp"

namespace style_corpus_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Style Corpus Search (Music composition and theory). Awaits wire-up.";
    return s;
}

}
