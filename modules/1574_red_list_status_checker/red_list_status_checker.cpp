#include "red_list_status_checker.hpp"

namespace red_list_status_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Red List status checker (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
