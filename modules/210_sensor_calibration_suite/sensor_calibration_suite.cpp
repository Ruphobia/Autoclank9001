#include "sensor_calibration_suite.hpp"

namespace sensor_calibration_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sensor calibration suite (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
