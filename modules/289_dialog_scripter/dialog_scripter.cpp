#include "dialog_scripter.hpp"

namespace dialog_scripter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dialog Scripter (Game development). Awaits wire-up.";
    return s;
}

}
