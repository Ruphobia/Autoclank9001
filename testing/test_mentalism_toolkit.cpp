#include "test_runner.hpp"
#include "../modules/1945_mentalism_toolkit/mentalism_toolkit.hpp"

namespace {

testing::TestOutcome run_mentalism_toolkit() {
    mentalism_toolkit::init();
    auto s = mentalism_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mentalism_toolkit::shutdown();
    return testing::ok();
}

const int _reg_mentalism_toolkit = testing::register_test(
    "mentalism_toolkit",
    "1945_mentalism_toolkit: stub status check",
    &run_mentalism_toolkit);

}
