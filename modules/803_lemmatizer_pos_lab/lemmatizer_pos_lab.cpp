#include "lemmatizer_pos_lab.hpp"

namespace lemmatizer_pos_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lemmatizer & POS Lab (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
