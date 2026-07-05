// SPDX-License-Identifier: GPL-3.0-or-later
#include "sourdough_starter_logger.hpp"

namespace sourdough_starter_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sourdough Starter Logger (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
