#include "roastprofilestudio.hpp"

namespace roastprofilestudio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RoastProfileStudio (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
