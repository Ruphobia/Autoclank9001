#include "test_runner.hpp"
#include "../modules/2146_source_citation_manager/source_citation_manager.hpp"

namespace {

testing::TestOutcome run_source_citation_manager() {
    source_citation_manager::init();
    auto s = source_citation_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    source_citation_manager::shutdown();
    return testing::ok();
}

const int _reg_source_citation_manager = testing::register_test(
    "source_citation_manager",
    "2146_source_citation_manager: stub status check",
    &run_source_citation_manager);

}
