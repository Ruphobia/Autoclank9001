// SPDX-License-Identifier: GPL-3.0-or-later
#include "field_telemetry_browser.hpp"

namespace field_telemetry_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Field telemetry browser (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
