#include "storyboard_and_xml_porter.hpp"

namespace storyboard_and_xml_porter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Storyboard and XML porter (Mobile development). Awaits wire-up.";
    return s;
}

}
