#include "test_runner.hpp"
#include "../modules/1886_provenance_and_pedigree_ledger/provenance_and_pedigree_ledger.hpp"

namespace {

testing::TestOutcome run_provenance_and_pedigree_ledger() {
    provenance_and_pedigree_ledger::init();
    auto s = provenance_and_pedigree_ledger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    provenance_and_pedigree_ledger::shutdown();
    return testing::ok();
}

const int _reg_provenance_and_pedigree_ledger = testing::register_test(
    "provenance_and_pedigree_ledger",
    "1886_provenance_and_pedigree_ledger: stub status check",
    &run_provenance_and_pedigree_ledger);

}
