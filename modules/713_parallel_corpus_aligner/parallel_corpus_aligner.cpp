#include "parallel_corpus_aligner.hpp"

namespace parallel_corpus_aligner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Parallel Corpus Aligner (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
