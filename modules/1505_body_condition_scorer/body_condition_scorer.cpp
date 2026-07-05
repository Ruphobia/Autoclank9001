#include "body_condition_scorer.hpp"

namespace body_condition_scorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Body condition scorer (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
