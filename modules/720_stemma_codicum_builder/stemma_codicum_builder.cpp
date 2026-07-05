#include "stemma_codicum_builder.hpp"

namespace stemma_codicum_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stemma Codicum Builder (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
