#include "gear_list_manager.hpp"

namespace gear_list_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gear List Manager (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
