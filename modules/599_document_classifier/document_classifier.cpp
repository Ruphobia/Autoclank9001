// SPDX-License-Identifier: GPL-3.0-or-later
#include "document_classifier.hpp"

namespace document_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Document Classifier (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
