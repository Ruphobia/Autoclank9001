#include "test_runner.hpp"
#include "../modules/182_build_orchestrator/build_orchestrator.hpp"

namespace {

testing::TestOutcome run_build_orchestrator() {
    build_orchestrator::init();
    auto s = build_orchestrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    build_orchestrator::shutdown();
    return testing::ok();
}

const int _reg_build_orchestrator = testing::register_test(
    "build_orchestrator",
    "182_build_orchestrator: stub status check",
    &run_build_orchestrator);

}
