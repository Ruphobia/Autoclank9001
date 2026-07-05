#include "ipa_reconstruction_lab.hpp"

namespace ipa_reconstruction_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: IPA Reconstruction Lab (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
