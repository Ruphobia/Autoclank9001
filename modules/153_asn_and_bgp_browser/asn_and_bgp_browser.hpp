#pragma once

#include <string>

// Stub module for the "ASN and BGP browser" tool entry.
// Web development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace asn_and_bgp_browser {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
