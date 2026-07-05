#include "test_runner.hpp"
#include "../modules/417_linked_notebook/linked_notebook.hpp"

namespace {

testing::TestOutcome run_linked_notebook() {
    linked_notebook::init();
    auto s = linked_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    linked_notebook::shutdown();
    return testing::ok();
}

const int _reg_linked_notebook = testing::register_test(
    "linked_notebook",
    "417_linked_notebook: stub status check",
    &run_linked_notebook);

}
