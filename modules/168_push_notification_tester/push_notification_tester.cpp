#include "push_notification_tester.hpp"

namespace push_notification_tester {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Push notification tester (Mobile development). Awaits wire-up.";
    return s;
}

}
