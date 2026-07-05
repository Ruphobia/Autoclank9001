#include "test_runner.hpp"
#include "../modules/715_epigraphy_workbench/epigraphy_workbench.hpp"

namespace {

testing::TestOutcome run_epigraphy_workbench() {
    epigraphy_workbench::init();
    auto s = epigraphy_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    epigraphy_workbench::shutdown();
    return testing::ok();
}

const int _reg_epigraphy_workbench = testing::register_test(
    "epigraphy_workbench",
    "715_epigraphy_workbench: stub status check",
    &run_epigraphy_workbench);

}
