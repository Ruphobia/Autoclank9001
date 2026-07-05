#include "ocr.hpp"

namespace ocr {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR (Image and video). Awaits wire-up.";
    return s;
}

}
