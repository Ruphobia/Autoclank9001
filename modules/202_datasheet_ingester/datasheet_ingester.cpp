#include "datasheet_ingester.hpp"

namespace datasheet_ingester {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Datasheet ingester (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
