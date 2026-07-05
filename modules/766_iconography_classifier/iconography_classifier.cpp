#include "iconography_classifier.hpp"

namespace iconography_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Iconography classifier (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
