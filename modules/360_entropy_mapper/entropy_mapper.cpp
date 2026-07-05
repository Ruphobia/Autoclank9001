// SPDX-License-Identifier: GPL-3.0-or-later
#include "entropy_mapper.hpp"

namespace entropy_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Entropy Mapper (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
