#include "license_picker.hpp"

namespace license_picker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: License Picker (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
