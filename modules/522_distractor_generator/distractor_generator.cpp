#include "distractor_generator.hpp"

namespace distractor_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Distractor Generator (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
