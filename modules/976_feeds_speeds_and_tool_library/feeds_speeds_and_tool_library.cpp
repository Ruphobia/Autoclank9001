#include "feeds_speeds_and_tool_library.hpp"

namespace feeds_speeds_and_tool_library {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Feeds, Speeds and Tool Library (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
