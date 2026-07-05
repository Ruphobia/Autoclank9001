// SPDX-License-Identifier: GPL-3.0-or-later
#include "kinetics_and_calibration_fitter.hpp"

namespace kinetics_and_calibration_fitter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Kinetics and calibration fitter (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
