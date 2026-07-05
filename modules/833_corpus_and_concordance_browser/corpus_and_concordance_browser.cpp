#include "corpus_and_concordance_browser.hpp"

namespace corpus_and_concordance_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Corpus and concordance browser (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
