#include "load_profile_builder.hpp"

namespace load_profile_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Load Profile Builder (Energy systems). Awaits wire-up.";
    return s;
}

}
