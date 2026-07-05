// SPDX-License-Identifier: GPL-3.0-or-later
#include "autograder.hpp"

namespace autograder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Autograder (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
