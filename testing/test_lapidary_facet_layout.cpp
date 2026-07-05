#include "test_runner.hpp"
#include "../modules/1847_lapidary_facet_layout/lapidary_facet_layout.hpp"

namespace {

testing::TestOutcome run_lapidary_facet_layout() {
    lapidary_facet_layout::init();
    auto s = lapidary_facet_layout::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lapidary_facet_layout::shutdown();
    return testing::ok();
}

const int _reg_lapidary_facet_layout = testing::register_test(
    "lapidary_facet_layout",
    "1847_lapidary_facet_layout: stub status check",
    &run_lapidary_facet_layout);

}
