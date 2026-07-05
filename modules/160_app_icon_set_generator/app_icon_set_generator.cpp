#include "app_icon_set_generator.hpp"

namespace app_icon_set_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: App icon set generator (Mobile development). Awaits wire-up.";
    return s;
}

}
