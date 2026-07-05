#include "test_runner.hpp"
#include "../modules/653_writing_and_citation_linter/writing_and_citation_linter.hpp"

namespace {

testing::TestOutcome run_writing_and_citation_linter() {
    writing_and_citation_linter::init();
    auto s = writing_and_citation_linter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    writing_and_citation_linter::shutdown();
    return testing::ok();
}

const int _reg_writing_and_citation_linter = testing::register_test(
    "writing_and_citation_linter",
    "653_writing_and_citation_linter: stub status check",
    &run_writing_and_citation_linter);

}
