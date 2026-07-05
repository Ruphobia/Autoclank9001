#include "id_ocr_pipeline.hpp"

namespace id_ocr_pipeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ID OCR Pipeline (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
