#include "highlight_exporter.hpp"

namespace highlight_exporter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Highlight exporter (Documents and reading). Awaits wire-up.";
    return s;
}

}
