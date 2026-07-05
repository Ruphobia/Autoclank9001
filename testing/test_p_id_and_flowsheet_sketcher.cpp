#include "test_runner.hpp"
#include "../modules/1121_p_id_and_flowsheet_sketcher/p_id_and_flowsheet_sketcher.hpp"

namespace {

testing::TestOutcome run_p_id_and_flowsheet_sketcher() {
    p_id_and_flowsheet_sketcher::init();
    auto s = p_id_and_flowsheet_sketcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    p_id_and_flowsheet_sketcher::shutdown();
    return testing::ok();
}

const int _reg_p_id_and_flowsheet_sketcher = testing::register_test(
    "p_id_and_flowsheet_sketcher",
    "1121_p_id_and_flowsheet_sketcher: stub status check",
    &run_p_id_and_flowsheet_sketcher);

}
