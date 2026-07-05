#include "layout_analyzer.hpp"

namespace layout_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Layout analyzer (Documents and reading). Awaits wire-up.";
    return s;
}

}
