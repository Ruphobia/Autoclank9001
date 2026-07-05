#include "test_runner.hpp"
#include "../modules/639_federated_paper_search/federated_paper_search.hpp"

namespace {

testing::TestOutcome run_federated_paper_search() {
    federated_paper_search::init();
    auto s = federated_paper_search::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    federated_paper_search::shutdown();
    return testing::ok();
}

const int _reg_federated_paper_search = testing::register_test(
    "federated_paper_search",
    "639_federated_paper_search: stub status check",
    &run_federated_paper_search);

}
