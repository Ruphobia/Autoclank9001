#include "manuscript_variant_apparatus.hpp"

namespace manuscript_variant_apparatus {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Manuscript Variant Apparatus (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
