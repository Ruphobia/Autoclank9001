#include "epub_reader.hpp"

namespace epub_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EPUB reader (Documents and reading). Awaits wire-up.";
    return s;
}

}
