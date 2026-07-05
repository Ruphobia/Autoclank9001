#include "test_runner.hpp"
#include "../modules/1700_crowd_director/crowd_director.hpp"

namespace {

testing::TestOutcome run_crowd_director() {
    crowd_director::init();
    auto s = crowd_director::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crowd_director::shutdown();
    return testing::ok();
}

const int _reg_crowd_director = testing::register_test(
    "crowd_director",
    "1700_crowd_director: stub status check",
    &run_crowd_director);

}
