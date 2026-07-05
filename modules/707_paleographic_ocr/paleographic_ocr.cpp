#include "paleographic_ocr.hpp"

namespace paleographic_ocr {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Paleographic OCR (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
