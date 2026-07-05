#include "test_runner.hpp"
#include "../modules/1375_mass_spec_and_proteomics_pipeline/mass_spec_and_proteomics_pipeline.hpp"

namespace {

testing::TestOutcome run_mass_spec_and_proteomics_pipeline() {
    mass_spec_and_proteomics_pipeline::init();
    auto s = mass_spec_and_proteomics_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mass_spec_and_proteomics_pipeline::shutdown();
    return testing::ok();
}

const int _reg_mass_spec_and_proteomics_pipeline = testing::register_test(
    "mass_spec_and_proteomics_pipeline",
    "1375_mass_spec_and_proteomics_pipeline: stub status check",
    &run_mass_spec_and_proteomics_pipeline);

}
