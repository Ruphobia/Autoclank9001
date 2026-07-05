#include "pcap_analyzer.hpp"

namespace pcap_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PCAP Analyzer (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
