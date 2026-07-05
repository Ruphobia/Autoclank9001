#include "test_runner.hpp"
#include "../modules/1985_route_notebook/route_notebook.hpp"

namespace {

testing::TestOutcome run_route_notebook() {
    route_notebook::init();
    auto s = route_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    route_notebook::shutdown();
    return testing::ok();
}

const int _reg_route_notebook = testing::register_test(
    "route_notebook",
    "1985_route_notebook: stub status check",
    &run_route_notebook);

}
