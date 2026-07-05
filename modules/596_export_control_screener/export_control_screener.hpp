#pragma once

#include <string>

// Stub module for the "Export Control Screener" tool entry.
// Legal, patents, trademarks, compliance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace export_control_screener {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
