#include "bios_and_firmware_catalog.hpp"

namespace bios_and_firmware_catalog {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BIOS and Firmware Catalog (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
