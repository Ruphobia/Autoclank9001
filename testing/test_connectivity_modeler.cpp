#include "test_runner.hpp"
#include "../modules/1566_connectivity_modeler/connectivity_modeler.hpp"

namespace {

testing::TestOutcome run_connectivity_modeler() {
    connectivity_modeler::init();
    auto s = connectivity_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    connectivity_modeler::shutdown();
    return testing::ok();
}

const int _reg_connectivity_modeler = testing::register_test(
    "connectivity_modeler",
    "1566_connectivity_modeler: stub status check",
    &run_connectivity_modeler);

}
