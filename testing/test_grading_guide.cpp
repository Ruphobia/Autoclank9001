#include "test_runner.hpp"
#include "../modules/1881_grading_guide/grading_guide.hpp"

namespace {

testing::TestOutcome run_grading_guide() {
    grading_guide::init();
    auto s = grading_guide::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grading_guide::shutdown();
    return testing::ok();
}

const int _reg_grading_guide = testing::register_test(
    "grading_guide",
    "1881_grading_guide: stub status check",
    &run_grading_guide);

}
