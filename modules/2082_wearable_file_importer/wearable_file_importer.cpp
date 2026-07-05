#include "wearable_file_importer.hpp"

namespace wearable_file_importer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wearable file importer (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
