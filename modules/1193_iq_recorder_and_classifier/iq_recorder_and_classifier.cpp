#include "iq_recorder_and_classifier.hpp"

namespace iq_recorder_and_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: IQ recorder and classifier (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
