#include "ioc_extractor_and_normalizer.hpp"

namespace ioc_extractor_and_normalizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: IOC Extractor and Normalizer (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
