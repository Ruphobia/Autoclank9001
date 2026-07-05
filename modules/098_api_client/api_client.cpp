// SPDX-License-Identifier: GPL-3.0-or-later
#include "api_client.hpp"

namespace api_client {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: API client (Software development). Awaits wire-up.";
    return s;
}

}
