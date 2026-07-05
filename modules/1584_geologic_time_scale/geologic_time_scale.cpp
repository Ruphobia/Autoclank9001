#include "geologic_time_scale.hpp"

namespace geologic_time_scale {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geologic Time Scale (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
