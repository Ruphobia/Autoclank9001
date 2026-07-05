#include "test_runner.hpp"
#include "../modules/1577_edna_metabarcoding_analyzer/edna_metabarcoding_analyzer.hpp"

namespace {

testing::TestOutcome run_edna_metabarcoding_analyzer() {
    edna_metabarcoding_analyzer::init();
    auto s = edna_metabarcoding_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    edna_metabarcoding_analyzer::shutdown();
    return testing::ok();
}

const int _reg_edna_metabarcoding_analyzer = testing::register_test(
    "edna_metabarcoding_analyzer",
    "1577_edna_metabarcoding_analyzer: stub status check",
    &run_edna_metabarcoding_analyzer);

}
