#include "frame_data_lookup.hpp"

namespace frame_data_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frame data lookup (Game playing assistants). Awaits wire-up.";
    return s;
}

}
