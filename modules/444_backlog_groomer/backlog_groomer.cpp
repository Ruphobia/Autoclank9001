#include "backlog_groomer.hpp"

namespace backlog_groomer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Backlog groomer (Project management). Awaits wire-up.";
    return s;
}

}
