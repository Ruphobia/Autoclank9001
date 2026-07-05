#include "test_runner.hpp"
#include "../modules/1694_sculpt_and_retopo/sculpt_and_retopo.hpp"

namespace {

testing::TestOutcome run_sculpt_and_retopo() {
    sculpt_and_retopo::init();
    auto s = sculpt_and_retopo::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sculpt_and_retopo::shutdown();
    return testing::ok();
}

const int _reg_sculpt_and_retopo = testing::register_test(
    "sculpt_and_retopo",
    "1694_sculpt_and_retopo: stub status check",
    &run_sculpt_and_retopo);

}
