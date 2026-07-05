#include "test_runner.hpp"
#include "../modules/1319_apparatus_and_glassware_sketcher/apparatus_and_glassware_sketcher.hpp"

namespace {

testing::TestOutcome run_apparatus_and_glassware_sketcher() {
    apparatus_and_glassware_sketcher::init();
    auto s = apparatus_and_glassware_sketcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    apparatus_and_glassware_sketcher::shutdown();
    return testing::ok();
}

const int _reg_apparatus_and_glassware_sketcher = testing::register_test(
    "apparatus_and_glassware_sketcher",
    "1319_apparatus_and_glassware_sketcher: stub status check",
    &run_apparatus_and_glassware_sketcher);

}
