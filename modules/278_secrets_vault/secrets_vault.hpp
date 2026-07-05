#pragma once

#include <string>

// Stub module for the "Secrets vault" tool entry.
// General computing and sysadmin.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace secrets_vault {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
