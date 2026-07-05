#include "value_and_notan_studio.hpp"

namespace value_and_notan_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Value and notan studio (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
