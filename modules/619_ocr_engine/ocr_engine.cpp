#include "ocr_engine.hpp"

namespace ocr_engine {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR engine (Documents and reading). Awaits wire-up.";
    return s;
}

}
