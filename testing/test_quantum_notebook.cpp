#include "test_runner.hpp"
#include "../modules/338_quantum_notebook/quantum_notebook.hpp"

namespace {

testing::TestOutcome run_quantum_notebook() {
    quantum_notebook::init();
    auto s = quantum_notebook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quantum_notebook::shutdown();
    return testing::ok();
}

const int _reg_quantum_notebook = testing::register_test(
    "quantum_notebook",
    "338_quantum_notebook: stub status check",
    &run_quantum_notebook);

}
