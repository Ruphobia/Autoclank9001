// SPDX-License-Identifier: GPL-3.0-or-later
#include "webhook_receiver.hpp"

namespace webhook_receiver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Webhook receiver (Web development). Awaits wire-up.";
    return s;
}

}
