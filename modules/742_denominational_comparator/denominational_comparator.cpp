#include "denominational_comparator.hpp"

namespace denominational_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Denominational Comparator (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
