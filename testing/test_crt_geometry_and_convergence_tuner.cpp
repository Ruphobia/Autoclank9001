#include "test_runner.hpp"
#include "../modules/411_crt_geometry_and_convergence_tuner/crt_geometry_and_convergence_tuner.hpp"

namespace {

testing::TestOutcome run_crt_geometry_and_convergence_tuner() {
    crt_geometry_and_convergence_tuner::init();
    auto s = crt_geometry_and_convergence_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crt_geometry_and_convergence_tuner::shutdown();
    return testing::ok();
}

const int _reg_crt_geometry_and_convergence_tuner = testing::register_test(
    "crt_geometry_and_convergence_tuner",
    "411_crt_geometry_and_convergence_tuner: stub status check",
    &run_crt_geometry_and_convergence_tuner);

}
