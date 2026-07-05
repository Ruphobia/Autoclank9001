#include "paraglider_site_database.hpp"

namespace paraglider_site_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Paraglider Site Database (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
