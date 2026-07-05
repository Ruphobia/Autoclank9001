#include "archival_finding_aid_browser.hpp"

namespace archival_finding_aid_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Archival finding-aid browser (History and archival research). Awaits wire-up.";
    return s;
}

}
