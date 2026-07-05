#include "test_runner.hpp"
#include "../modules/546_glm_rate_modeler/glm_rate_modeler.hpp"

namespace {

testing::TestOutcome run_glm_rate_modeler() {
    glm_rate_modeler::init();
    auto s = glm_rate_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    glm_rate_modeler::shutdown();
    return testing::ok();
}

const int _reg_glm_rate_modeler = testing::register_test(
    "glm_rate_modeler",
    "546_glm_rate_modeler: stub status check",
    &run_glm_rate_modeler);

}
