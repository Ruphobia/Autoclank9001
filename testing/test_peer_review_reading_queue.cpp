#include "test_runner.hpp"
#include "../modules/645_peer_review_reading_queue/peer_review_reading_queue.hpp"

namespace {

testing::TestOutcome run_peer_review_reading_queue() {
    peer_review_reading_queue::init();
    auto s = peer_review_reading_queue::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    peer_review_reading_queue::shutdown();
    return testing::ok();
}

const int _reg_peer_review_reading_queue = testing::register_test(
    "peer_review_reading_queue",
    "645_peer_review_reading_queue: stub status check",
    &run_peer_review_reading_queue);

}
