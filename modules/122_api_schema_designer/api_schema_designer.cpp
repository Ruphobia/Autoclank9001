#include "api_schema_designer.hpp"

namespace api_schema_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: API schema designer (Web development). Awaits wire-up.";
    return s;
}

}
