#include "test_runner.hpp"
#include "../modules/1685_rigging_studio/rigging_studio.hpp"

namespace {

testing::TestOutcome run_rigging_studio() {
    rigging_studio::init();
    auto s = rigging_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rigging_studio::shutdown();
    return testing::ok();
}

const int _reg_rigging_studio = testing::register_test(
    "rigging_studio",
    "1685_rigging_studio: stub status check",
    &run_rigging_studio);

}
