#include "document_summarizer.hpp"

namespace document_summarizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Document summarizer (Documents and reading). Awaits wire-up.";
    return s;
}

}
