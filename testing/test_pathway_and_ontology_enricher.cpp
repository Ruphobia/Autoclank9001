#include "test_runner.hpp"
#include "../modules/1377_pathway_and_ontology_enricher/pathway_and_ontology_enricher.hpp"

namespace {

testing::TestOutcome run_pathway_and_ontology_enricher() {
    pathway_and_ontology_enricher::init();
    auto s = pathway_and_ontology_enricher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pathway_and_ontology_enricher::shutdown();
    return testing::ok();
}

const int _reg_pathway_and_ontology_enricher = testing::register_test(
    "pathway_and_ontology_enricher",
    "1377_pathway_and_ontology_enricher: stub status check",
    &run_pathway_and_ontology_enricher);

}
