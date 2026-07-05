#include "test_runner.hpp"
#include "../modules/296_skeletal_rigger/skeletal_rigger.hpp"

namespace {

testing::TestOutcome run_skeletal_rigger() {
    skeletal_rigger::init();
    auto s = skeletal_rigger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    skeletal_rigger::shutdown();
    return testing::ok();
}

const int _reg_skeletal_rigger = testing::register_test(
    "skeletal_rigger",
    "296_skeletal_rigger: stub status check",
    &run_skeletal_rigger);

}
