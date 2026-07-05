#include "scribal_hand_classifier.hpp"

namespace scribal_hand_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scribal Hand Classifier (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
