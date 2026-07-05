#include "test_runner.hpp"
#include "../modules/620_math_and_equation_ocr/math_and_equation_ocr.hpp"

namespace {

testing::TestOutcome run_math_and_equation_ocr() {
    math_and_equation_ocr::init();
    auto s = math_and_equation_ocr::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    math_and_equation_ocr::shutdown();
    return testing::ok();
}

const int _reg_math_and_equation_ocr = testing::register_test(
    "math_and_equation_ocr",
    "620_math_and_equation_ocr: stub status check",
    &run_math_and_equation_ocr);

}
