#include "sensor_stream_viewer.hpp"

namespace sensor_stream_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sensor stream viewer (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
