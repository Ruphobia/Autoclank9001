#include "morphological_analyzer.hpp"

namespace morphological_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Morphological analyzer (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
