// SPDX-License-Identifier: GPL-3.0-or-later
#include "geocoder.hpp"

namespace geocoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geocoder (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
