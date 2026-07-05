#include "form_builder.hpp"

namespace form_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Form builder (Web development). Awaits wire-up.";
    return s;
}

}
