#include "test_runner.hpp"
#include "../modules/1508_species_distribution_modeler/species_distribution_modeler.hpp"

namespace {

testing::TestOutcome run_species_distribution_modeler() {
    species_distribution_modeler::init();
    auto s = species_distribution_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    species_distribution_modeler::shutdown();
    return testing::ok();
}

const int _reg_species_distribution_modeler = testing::register_test(
    "species_distribution_modeler",
    "1508_species_distribution_modeler: stub status check",
    &run_species_distribution_modeler);

}
