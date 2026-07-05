#include "precinct_to_district_joiner.hpp"

namespace precinct_to_district_joiner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Precinct-to-district joiner (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
