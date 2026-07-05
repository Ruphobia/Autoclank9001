#include "test_runner.hpp"
#include "../modules/784_citation_and_provenance_manager/citation_and_provenance_manager.hpp"

namespace {

testing::TestOutcome run_citation_and_provenance_manager() {
    citation_and_provenance_manager::init();
    auto s = citation_and_provenance_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    citation_and_provenance_manager::shutdown();
    return testing::ok();
}

const int _reg_citation_and_provenance_manager = testing::register_test(
    "citation_and_provenance_manager",
    "784_citation_and_provenance_manager: stub status check",
    &run_citation_and_provenance_manager);

}
