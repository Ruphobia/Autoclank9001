#include "ros_bag_and_mcap_inspector.hpp"

namespace ros_bag_and_mcap_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ROS bag and MCAP inspector (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
