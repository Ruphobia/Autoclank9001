#include "annotation_and_margin_notes_layer.hpp"

namespace annotation_and_margin_notes_layer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Annotation and Margin-Notes Layer (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
