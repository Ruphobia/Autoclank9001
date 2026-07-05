#include "ocr_for_drawings.hpp"

namespace ocr_for_drawings {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR for drawings (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
