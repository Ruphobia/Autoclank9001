#include "rom_dumper_station.hpp"

namespace rom_dumper_station {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ROM Dumper Station (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
