#include "test_runner.hpp"
#include "../modules/2048_taxidermy_notebook/taxidermy_notebook.hpp"

namespace {

testing::TestOutcome run_taxidermy_notebook() {
    taxidermy_notebook::init();
    auto s = taxidermy_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    taxidermy_notebook::shutdown();
    return testing::ok();
}

const int _reg_taxidermy_notebook = testing::register_test(
    "taxidermy_notebook",
    "2048_taxidermy_notebook: stub status check",
    &run_taxidermy_notebook);

}
