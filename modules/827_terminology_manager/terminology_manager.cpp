#include "terminology_manager.hpp"

namespace terminology_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Terminology manager (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
