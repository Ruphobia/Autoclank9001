// SPDX-License-Identifier: GPL-3.0-or-later
#include "trace_and_profiler.hpp"

namespace trace_and_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Trace and profiler (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
