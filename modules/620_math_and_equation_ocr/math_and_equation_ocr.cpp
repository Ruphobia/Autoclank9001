#include "math_and_equation_ocr.hpp"

namespace math_and_equation_ocr {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Math and equation OCR (Documents and reading). Awaits wire-up.";
    return s;
}

}
