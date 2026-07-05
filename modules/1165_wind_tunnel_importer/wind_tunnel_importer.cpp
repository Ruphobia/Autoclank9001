#include "wind_tunnel_importer.hpp"

namespace wind_tunnel_importer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wind Tunnel Importer (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
