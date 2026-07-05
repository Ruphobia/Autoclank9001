#include "test_runner.hpp"
#include "../modules/724_marginalia_notebook/marginalia_notebook.hpp"

namespace {

testing::TestOutcome run_marginalia_notebook() {
    marginalia_notebook::init();
    auto s = marginalia_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    marginalia_notebook::shutdown();
    return testing::ok();
}

const int _reg_marginalia_notebook = testing::register_test(
    "marginalia_notebook",
    "724_marginalia_notebook: stub status check",
    &run_marginalia_notebook);

}
