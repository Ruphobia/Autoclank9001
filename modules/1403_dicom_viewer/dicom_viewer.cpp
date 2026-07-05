#include "dicom_viewer.hpp"

namespace dicom_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DICOM Viewer (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
