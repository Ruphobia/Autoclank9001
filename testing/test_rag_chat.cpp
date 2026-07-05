// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/626_rag_chat/rag_chat.hpp"

namespace {

testing::TestOutcome run_rag_chat() {
    rag_chat::init();
    auto s = rag_chat::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rag_chat::shutdown();
    return testing::ok();
}

const int _reg_rag_chat = testing::register_test(
    "rag_chat",
    "626_rag_chat: stub status check",
    &run_rag_chat);

}
