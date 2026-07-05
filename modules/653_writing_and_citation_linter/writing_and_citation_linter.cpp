#include "writing_and_citation_linter.hpp"

namespace writing_and_citation_linter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Writing and Citation Linter (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
