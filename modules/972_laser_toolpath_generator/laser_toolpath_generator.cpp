#include "laser_toolpath_generator.hpp"

namespace laser_toolpath_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Laser Toolpath Generator (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
