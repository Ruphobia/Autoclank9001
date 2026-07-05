#include "test_runner.hpp"
#include "../modules/1279_load_profile_builder/load_profile_builder.hpp"

namespace {

testing::TestOutcome run_load_profile_builder() {
    load_profile_builder::init();
    auto s = load_profile_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    load_profile_builder::shutdown();
    return testing::ok();
}

const int _reg_load_profile_builder = testing::register_test(
    "load_profile_builder",
    "1279_load_profile_builder: stub status check",
    &run_load_profile_builder);

}
