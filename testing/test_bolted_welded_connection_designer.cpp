#include "test_runner.hpp"
#include "../modules/1054_bolted_welded_connection_designer/bolted_welded_connection_designer.hpp"

namespace {

testing::TestOutcome run_bolted_welded_connection_designer() {
    bolted_welded_connection_designer::init();
    auto s = bolted_welded_connection_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bolted_welded_connection_designer::shutdown();
    return testing::ok();
}

const int _reg_bolted_welded_connection_designer = testing::register_test(
    "bolted_welded_connection_designer",
    "1054_bolted_welded_connection_designer: stub status check",
    &run_bolted_welded_connection_designer);

}
