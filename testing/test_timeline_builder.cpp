#include "test_runner.hpp"
#include "../modules/368_timeline_builder/timeline_builder.hpp"

namespace {

testing::TestOutcome run_timeline_builder() {
    timeline_builder::init();
    auto s = timeline_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    timeline_builder::shutdown();
    return testing::ok();
}

const int _reg_timeline_builder = testing::register_test(
    "timeline_builder",
    "368_timeline_builder: stub status check",
    &run_timeline_builder);

}
