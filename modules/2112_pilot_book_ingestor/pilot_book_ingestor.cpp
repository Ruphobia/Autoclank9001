#include "pilot_book_ingestor.hpp"

namespace pilot_book_ingestor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pilot book ingestor (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
