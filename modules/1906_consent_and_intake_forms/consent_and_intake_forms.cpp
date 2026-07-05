#include "consent_and_intake_forms.hpp"

namespace consent_and_intake_forms {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Consent and Intake Forms (Body art: tattoo and body modification). Awaits wire-up.";
    return s;
}

}
