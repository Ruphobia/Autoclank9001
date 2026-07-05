#include "receipt_document_capture.hpp"

namespace receipt_document_capture {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Receipt & Document Capture (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
