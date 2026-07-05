// SPDX-License-Identifier: GPL-3.0-or-later
#include "crt_geometry_and_convergence_tuner.hpp"

namespace crt_geometry_and_convergence_tuner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CRT Geometry and Convergence Tuner (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
