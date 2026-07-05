#pragma once

#include <string>

// Stub module for the "File Carver" tool entry.
// Cybersecurity and digital forensics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace file_carver {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
