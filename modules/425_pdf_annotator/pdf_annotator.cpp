#include "pdf_annotator.hpp"

namespace pdf_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PDF annotator (Office productivity). Awaits wire-up.";
    return s;
}

}
