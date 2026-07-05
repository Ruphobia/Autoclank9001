// SPDX-License-Identifier: GPL-3.0-or-later
#include "classroom_runner.hpp"

namespace classroom_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Classroom Runner (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
