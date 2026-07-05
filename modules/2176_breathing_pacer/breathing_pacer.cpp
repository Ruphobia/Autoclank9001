#include "breathing_pacer.hpp"

namespace breathing_pacer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Breathing pacer (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
