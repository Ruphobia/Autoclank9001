#include "paleography_ocr.hpp"

namespace paleography_ocr {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Paleography OCR (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
