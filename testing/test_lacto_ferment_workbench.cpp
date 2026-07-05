#include "test_runner.hpp"
#include "../modules/1794_lacto_ferment_workbench/lacto_ferment_workbench.hpp"

namespace {

testing::TestOutcome run_lacto_ferment_workbench() {
    lacto_ferment_workbench::init();
    auto s = lacto_ferment_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lacto_ferment_workbench::shutdown();
    return testing::ok();
}

const int _reg_lacto_ferment_workbench = testing::register_test(
    "lacto_ferment_workbench",
    "1794_lacto_ferment_workbench: stub status check",
    &run_lacto_ferment_workbench);

}
