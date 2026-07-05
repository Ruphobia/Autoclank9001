// SPDX-License-Identifier: GPL-3.0-or-later
#include "redis_client.hpp"

namespace redis_client {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Redis client (Databases). Awaits wire-up.";
    return s;
}

}
