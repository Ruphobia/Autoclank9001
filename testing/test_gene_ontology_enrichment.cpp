#include "test_runner.hpp"
#include "../modules/1336_gene_ontology_enrichment/gene_ontology_enrichment.hpp"

namespace {

testing::TestOutcome run_gene_ontology_enrichment() {
    gene_ontology_enrichment::init();
    auto s = gene_ontology_enrichment::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gene_ontology_enrichment::shutdown();
    return testing::ok();
}

const int _reg_gene_ontology_enrichment = testing::register_test(
    "gene_ontology_enrichment",
    "1336_gene_ontology_enrichment: stub status check",
    &run_gene_ontology_enrichment);

}
