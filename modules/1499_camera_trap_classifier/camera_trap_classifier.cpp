#include "camera_trap_classifier.hpp"

namespace camera_trap_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Camera trap classifier (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
