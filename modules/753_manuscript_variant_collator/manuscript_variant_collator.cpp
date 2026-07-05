#include "manuscript_variant_collator.hpp"

namespace manuscript_variant_collator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Manuscript variant collator (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
