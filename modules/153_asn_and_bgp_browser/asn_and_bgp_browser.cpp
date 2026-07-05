#include "asn_and_bgp_browser.hpp"

namespace asn_and_bgp_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ASN and BGP browser (Web development). Awaits wire-up.";
    return s;
}

}
