#include "stemma_builder.hpp"

namespace stemma_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stemma builder (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
