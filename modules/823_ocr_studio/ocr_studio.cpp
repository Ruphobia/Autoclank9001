#include "ocr_studio.hpp"

namespace ocr_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR studio (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
