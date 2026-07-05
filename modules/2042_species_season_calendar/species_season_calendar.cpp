#include "species_season_calendar.hpp"

namespace species_season_calendar {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Species Season Calendar (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
