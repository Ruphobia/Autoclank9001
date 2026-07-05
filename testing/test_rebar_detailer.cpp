#include "test_runner.hpp"
#include "../modules/1053_rebar_detailer/rebar_detailer.hpp"

namespace {

testing::TestOutcome run_rebar_detailer() {
    rebar_detailer::init();
    auto s = rebar_detailer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rebar_detailer::shutdown();
    return testing::ok();
}

const int _reg_rebar_detailer = testing::register_test(
    "rebar_detailer",
    "1053_rebar_detailer: stub status check",
    &run_rebar_detailer);

}
