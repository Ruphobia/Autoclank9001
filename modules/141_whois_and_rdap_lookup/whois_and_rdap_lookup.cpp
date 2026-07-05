#include "whois_and_rdap_lookup.hpp"

namespace whois_and_rdap_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: WHOIS and RDAP lookup (Web development). Awaits wire-up.";
    return s;
}

}
