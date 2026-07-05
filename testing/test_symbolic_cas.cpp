#include "test_runner.hpp"
#include "../modules/891_symbolic_cas/symbolic_cas.hpp"

namespace {

testing::TestOutcome run_symbolic_cas() {
    symbolic_cas::init();
    auto s = symbolic_cas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    symbolic_cas::shutdown();
    return testing::ok();
}

const int _reg_symbolic_cas = testing::register_test(
    "symbolic_cas",
    "891_symbolic_cas: stub status check",
    &run_symbolic_cas);

}
