#include "citizen_science_submission_helper.hpp"

namespace citizen_science_submission_helper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Citizen-science submission helper (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
