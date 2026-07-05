#include "test_runner.hpp"
#include "../modules/344_qec_workbench/qec_workbench.hpp"

namespace {

testing::TestOutcome run_qec_workbench() {
    qec_workbench::init();
    auto s = qec_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    qec_workbench::shutdown();
    return testing::ok();
}

const int _reg_qec_workbench = testing::register_test(
    "qec_workbench",
    "344_qec_workbench: stub status check",
    &run_qec_workbench);

}
