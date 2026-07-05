#include "mesh_reconstruction_viewer.hpp"

namespace mesh_reconstruction_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mesh Reconstruction Viewer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
