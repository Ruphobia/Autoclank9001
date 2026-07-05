#include "figure_and_diagram_extractor.hpp"

namespace figure_and_diagram_extractor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Figure and diagram extractor (Documents and reading). Awaits wire-up.";
    return s;
}

}
