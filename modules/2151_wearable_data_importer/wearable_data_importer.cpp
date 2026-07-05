#include "wearable_data_importer.hpp"

namespace wearable_data_importer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wearable Data Importer (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
