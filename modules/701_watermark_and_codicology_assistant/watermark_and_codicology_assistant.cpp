#include "watermark_and_codicology_assistant.hpp"

namespace watermark_and_codicology_assistant {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Watermark and codicology assistant (History and archival research). Awaits wire-up.";
    return s;
}

}
