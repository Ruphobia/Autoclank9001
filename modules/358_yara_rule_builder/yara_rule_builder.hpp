#pragma once

#include <string>

// Stub module for the "YARA Rule Builder" tool entry.
// Cybersecurity and digital forensics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace yara_rule_builder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
