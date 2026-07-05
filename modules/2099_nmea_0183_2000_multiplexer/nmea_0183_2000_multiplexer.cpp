#include "nmea_0183_2000_multiplexer.hpp"

namespace nmea_0183_2000_multiplexer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NMEA 0183/2000 multiplexer (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
