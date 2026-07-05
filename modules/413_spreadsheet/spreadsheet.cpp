// SPDX-License-Identifier: GPL-3.0-or-later
#include "spreadsheet.hpp"

namespace spreadsheet {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spreadsheet (Office productivity). Awaits wire-up.";
    return s;
}

}
