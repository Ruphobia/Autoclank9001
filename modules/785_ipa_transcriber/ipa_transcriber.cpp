#include "ipa_transcriber.hpp"

namespace ipa_transcriber {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: IPA Transcriber (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
