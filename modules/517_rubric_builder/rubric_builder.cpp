#include "rubric_builder.hpp"

namespace rubric_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rubric Builder (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
