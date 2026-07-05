#include "historical_ner_tagger.hpp"

namespace historical_ner_tagger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Historical NER tagger (History and archival research). Awaits wire-up.";
    return s;
}

}
