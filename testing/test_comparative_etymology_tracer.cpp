#include "test_runner.hpp"
#include "../modules/773_comparative_etymology_tracer/comparative_etymology_tracer.hpp"

namespace {

testing::TestOutcome run_comparative_etymology_tracer() {
    comparative_etymology_tracer::init();
    auto s = comparative_etymology_tracer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comparative_etymology_tracer::shutdown();
    return testing::ok();
}

const int _reg_comparative_etymology_tracer = testing::register_test(
    "comparative_etymology_tracer",
    "773_comparative_etymology_tracer: stub status check",
    &run_comparative_etymology_tracer);

}
