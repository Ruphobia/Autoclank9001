#include "test_runner.hpp"
#include "../modules/513_public_records_desk/public_records_desk.hpp"

namespace {

testing::TestOutcome run_public_records_desk() {
    public_records_desk::init();
    auto s = public_records_desk::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    public_records_desk::shutdown();
    return testing::ok();
}

const int _reg_public_records_desk = testing::register_test(
    "public_records_desk",
    "513_public_records_desk: stub status check",
    &run_public_records_desk);

}
