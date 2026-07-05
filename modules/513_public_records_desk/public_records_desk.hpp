#pragma once

#include <string>

// Stub module for the "Public-records desk" tool entry.
// Writing, publishing, journalism.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace public_records_desk {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
