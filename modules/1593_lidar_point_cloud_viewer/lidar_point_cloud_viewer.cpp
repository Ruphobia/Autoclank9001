#include "lidar_point_cloud_viewer.hpp"

namespace lidar_point_cloud_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: LiDAR Point Cloud Viewer (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
