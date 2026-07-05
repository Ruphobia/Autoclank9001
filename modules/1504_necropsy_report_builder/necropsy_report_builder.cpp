#include "necropsy_report_builder.hpp"

namespace necropsy_report_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Necropsy report builder (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
