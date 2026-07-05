#include "test_runner.hpp"
#include "../modules/640_bibtex_csl_manager/bibtex_csl_manager.hpp"

namespace {

testing::TestOutcome run_bibtex_csl_manager() {
    bibtex_csl_manager::init();
    auto s = bibtex_csl_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bibtex_csl_manager::shutdown();
    return testing::ok();
}

const int _reg_bibtex_csl_manager = testing::register_test(
    "bibtex_csl_manager",
    "640_bibtex_csl_manager: stub status check",
    &run_bibtex_csl_manager);

}
