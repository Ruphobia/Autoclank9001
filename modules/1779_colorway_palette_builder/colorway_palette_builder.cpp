// SPDX-License-Identifier: GPL-3.0-or-later
#include "colorway_palette_builder.hpp"

namespace colorway_palette_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Colorway Palette Builder (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
