#pragma once

#include <string>

// Stub module for the "OCR and Scan Cleanup" tool entry.
// Memory and knowledge accrual.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ocr_and_scan_cleanup {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
