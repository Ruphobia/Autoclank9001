#include "test_runner.hpp"
#include "../modules/865_connector_and_cable_designer/connector_and_cable_designer.hpp"

namespace {

testing::TestOutcome run_connector_and_cable_designer() {
    connector_and_cable_designer::init();
    auto s = connector_and_cable_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    connector_and_cable_designer::shutdown();
    return testing::ok();
}

const int _reg_connector_and_cable_designer = testing::register_test(
    "connector_and_cable_designer",
    "865_connector_and_cable_designer: stub status check",
    &run_connector_and_cable_designer);

}
