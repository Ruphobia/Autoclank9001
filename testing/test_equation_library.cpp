#include "test_runner.hpp"
#include "../modules/895_equation_library/equation_library.hpp"

namespace {

testing::TestOutcome run_equation_library() {
    equation_library::init();
    auto s = equation_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    equation_library::shutdown();
    return testing::ok();
}

const int _reg_equation_library = testing::register_test(
    "equation_library",
    "895_equation_library: stub status check",
    &run_equation_library);

}
