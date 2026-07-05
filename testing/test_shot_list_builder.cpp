#include "test_runner.hpp"
#include "../modules/1670_shot_list_builder/shot_list_builder.hpp"

namespace {

testing::TestOutcome run_shot_list_builder() {
    shot_list_builder::init();
    auto s = shot_list_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    shot_list_builder::shutdown();
    return testing::ok();
}

const int _reg_shot_list_builder = testing::register_test(
    "shot_list_builder",
    "1670_shot_list_builder: stub status check",
    &run_shot_list_builder);

}
