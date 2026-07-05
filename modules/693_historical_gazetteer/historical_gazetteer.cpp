#include "historical_gazetteer.hpp"

namespace historical_gazetteer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Historical gazetteer (History and archival research). Awaits wire-up.";
    return s;
}

}
