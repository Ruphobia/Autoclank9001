// SPDX-License-Identifier: GPL-3.0-or-later
#include "standards_aligner.hpp"

namespace standards_aligner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Standards Aligner (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
