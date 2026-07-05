#include "ocr_scanner.hpp"

namespace ocr_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR scanner (Office productivity). Awaits wire-up.";
    return s;
}

}
