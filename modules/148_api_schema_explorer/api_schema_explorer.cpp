#include "api_schema_explorer.hpp"

namespace api_schema_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: API schema explorer (Web development). Awaits wire-up.";
    return s;
}

}
