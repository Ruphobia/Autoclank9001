#include "submission_tracker.hpp"

namespace submission_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Submission tracker (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
