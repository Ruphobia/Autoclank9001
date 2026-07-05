#include "test_runner.hpp"
#include "../modules/1598_field_notebook/field_notebook.hpp"

namespace {

testing::TestOutcome run_field_notebook() {
    field_notebook::init();
    auto s = field_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    field_notebook::shutdown();
    return testing::ok();
}

const int _reg_field_notebook = testing::register_test(
    "field_notebook",
    "1598_field_notebook: stub status check",
    &run_field_notebook);

}
