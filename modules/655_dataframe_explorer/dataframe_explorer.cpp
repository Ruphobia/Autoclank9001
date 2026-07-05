// SPDX-License-Identifier: GPL-3.0-or-later
#include "dataframe_explorer.hpp"

namespace dataframe_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dataframe explorer (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
