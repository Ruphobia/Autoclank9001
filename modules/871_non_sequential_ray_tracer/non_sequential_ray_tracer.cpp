#include "non_sequential_ray_tracer.hpp"

namespace non_sequential_ray_tracer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Non-Sequential Ray Tracer (Optics and photonics). Awaits wire-up.";
    return s;
}

}
