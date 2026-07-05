#include "test_runner.hpp"
#include "../modules/2062_knot_reference_library/knot_reference_library.hpp"

namespace {

testing::TestOutcome run_knot_reference_library() {
    knot_reference_library::init();
    auto s = knot_reference_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    knot_reference_library::shutdown();
    return testing::ok();
}

const int _reg_knot_reference_library = testing::register_test(
    "knot_reference_library",
    "2062_knot_reference_library: stub status check",
    &run_knot_reference_library);

}
