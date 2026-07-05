#include "test_runner.hpp"
#include "../modules/572_lbo_m_a_modeler/lbo_m_a_modeler.hpp"

namespace {

testing::TestOutcome run_lbo_m_a_modeler() {
    lbo_m_a_modeler::init();
    auto s = lbo_m_a_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lbo_m_a_modeler::shutdown();
    return testing::ok();
}

const int _reg_lbo_m_a_modeler = testing::register_test(
    "lbo_m_a_modeler",
    "572_lbo_m_a_modeler: stub status check",
    &run_lbo_m_a_modeler);

}
