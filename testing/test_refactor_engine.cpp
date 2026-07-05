#include "test_runner.hpp"
#include "../modules/088_refactor_engine/refactor_engine.hpp"

namespace {

testing::TestOutcome run_refactor_engine() {
    refactor_engine::init();
    auto s = refactor_engine::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    refactor_engine::shutdown();
    return testing::ok();
}

const int _reg_refactor_engine = testing::register_test(
    "refactor_engine",
    "088_refactor_engine: stub status check",
    &run_refactor_engine);

}
