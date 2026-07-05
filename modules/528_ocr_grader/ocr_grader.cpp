#include "ocr_grader.hpp"

namespace ocr_grader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR Grader (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
