// SPDX-License-Identifier: GPL-3.0-or-later
#include "bayesian_belief_sandbox.hpp"

namespace bayesian_belief_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bayesian belief sandbox (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
