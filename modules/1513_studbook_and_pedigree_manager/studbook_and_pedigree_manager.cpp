#include "studbook_and_pedigree_manager.hpp"

namespace studbook_and_pedigree_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Studbook and pedigree manager (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
