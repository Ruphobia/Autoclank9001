#pragma once

#include <string>

// Stub module for the "Subdomain and asset enumerator" tool entry.
// Web development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace subdomain_and_asset_enumerator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
