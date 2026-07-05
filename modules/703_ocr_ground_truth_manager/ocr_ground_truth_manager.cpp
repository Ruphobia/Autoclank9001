#include "ocr_ground_truth_manager.hpp"

namespace ocr_ground_truth_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OCR ground-truth manager (History and archival research). Awaits wire-up.";
    return s;
}

}
