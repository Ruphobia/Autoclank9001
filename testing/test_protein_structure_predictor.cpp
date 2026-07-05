#include "test_runner.hpp"
#include "../modules/1362_protein_structure_predictor/protein_structure_predictor.hpp"

namespace {

testing::TestOutcome run_protein_structure_predictor() {
    protein_structure_predictor::init();
    auto s = protein_structure_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    protein_structure_predictor::shutdown();
    return testing::ok();
}

const int _reg_protein_structure_predictor = testing::register_test(
    "protein_structure_predictor",
    "1362_protein_structure_predictor: stub status check",
    &run_protein_structure_predictor);

}
