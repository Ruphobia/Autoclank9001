#include "socratic_tutor.hpp"

namespace socratic_tutor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Socratic Tutor (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
