#include "test_runner.hpp"
#include "../modules/1975_frame_data_lookup/frame_data_lookup.hpp"

namespace {

testing::TestOutcome run_frame_data_lookup() {
    frame_data_lookup::init();
    auto s = frame_data_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frame_data_lookup::shutdown();
    return testing::ok();
}

const int _reg_frame_data_lookup = testing::register_test(
    "frame_data_lookup",
    "1975_frame_data_lookup: stub status check",
    &run_frame_data_lookup);

}
