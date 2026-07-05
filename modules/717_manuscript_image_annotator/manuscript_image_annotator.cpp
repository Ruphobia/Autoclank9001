#include "manuscript_image_annotator.hpp"

namespace manuscript_image_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Manuscript Image Annotator (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
