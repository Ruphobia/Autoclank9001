#include "body_scan_and_yoga_nidra_player.hpp"

namespace body_scan_and_yoga_nidra_player {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Body scan and yoga nidra player (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
