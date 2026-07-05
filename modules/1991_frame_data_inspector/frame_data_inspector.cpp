#include "frame_data_inspector.hpp"

namespace frame_data_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frame Data Inspector (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
