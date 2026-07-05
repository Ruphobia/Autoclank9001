#include "seed_data_generator.hpp"

namespace seed_data_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Seed data generator (Databases). Awaits wire-up.";
    return s;
}

}
