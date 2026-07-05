#include "test_runner.hpp"
#include "../modules/500_prose_linter/prose_linter.hpp"

namespace {

testing::TestOutcome run_prose_linter() {
    prose_linter::init();
    auto s = prose_linter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prose_linter::shutdown();
    return testing::ok();
}

const int _reg_prose_linter = testing::register_test(
    "prose_linter",
    "500_prose_linter: stub status check",
    &run_prose_linter);

}
