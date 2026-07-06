// SPDX-License-Identifier: GPL-3.0-or-later
// ac9 — main entry. Brings up the web UI immediately, then loads the
// LLM stack in a background thread while updating the shared status block
// the UI polls via /api/status.

#include "001_prompt_cleanup/cleanup.hpp"
#include "002_dictionary/dictionary.hpp"
#include "003_stylize/stylize.hpp"
#include "004_expertise/expertise.hpp"
#include "005_context/context.hpp"
#include "006_disambiguate/disambiguate.hpp"
#include "007_knowledge/kb.hpp"
#include "008_entities/entities.hpp"
#include "009_tools/classify.hpp"
#include "009_tools/answer.hpp"
#include "010_interface/server.hpp"
#include "010_interface/sessions_store.hpp"
#include "010_interface/status.hpp"
#include "012_hardware/hardware.hpp"
#include "data/data.hpp"
#include "data/bootstrap.hpp"

#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <exception>
#include <thread>

namespace {

void load_pipeline_background() {
    try {
        status::set_overall("opening session memory", false);
        status::note("context", "loading");
        context::init();
        // Reclaim ghost sessions planted by prior /api/context/clear
        // invocations before we added the reuse-empty guard (older than
        // 2 hours, no metadata, zero rows, not currently active).
        sessions_store::sweep_empty_ghosts(2LL * 3600 * 1000);
        status::note("context", "ready");

        status::set_overall("loading dictionary", false);
        status::note("dictionary", "loading", "Webster 1913 + WordNet");
        dictionary::init();
        status::note("dictionary", "ready");

        status::set_overall("loading cleanup model (1.5B)", false);
        status::note("cleanup", "loading", "Qwen2.5-1.5B Q8_0 on GPU 0");
        prompt_cleanup::init();
        status::note("cleanup", "ready");

        status::set_overall("loading 14B model on GPU 0", false);
        status::note("qwen14b", "loading", "Qwen2.5-14B Q5_K_M (~15s)");
        stylize::init();      // also covers expertise/disambiguate/entities/classify/answer
        expertise::init();
        disambiguate::init();
        entities::init();
        classify::init();
        answer::init();
        status::note("qwen14b", "ready");

        status::set_overall("starting Wikipedia knowledge base", false);
        status::note("kb", "loading", "background");
        kb::init();
        // kb is non-blocking; mark loading regardless. A second status pass
        // below polls kb status and updates accordingly.
        status::note("kb", kb::status_string().c_str());

        status::set_overall("ready", true);

        // Keep kb status fresh while it's downloading.
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            const std::string s = kb::status_string();
            status::note("kb", s);
            if (s.find("ready") != std::string::npos ||
                s.find("offline") != std::string::npos ||
                s.find("error") != std::string::npos) {
                break;
            }
        }
    } catch (const std::exception & ex) {
        status::note("error", "fatal", ex.what());
        status::set_overall(std::string("error: ") + ex.what(), false);
        std::fprintf(stderr, "ac9: pipeline load error: %s\n", ex.what());
    }
}

}

int main(int argc, char ** argv) {
    // Subcommand dispatch. `ac9` with no args runs the server; `ac9 chunk
    // ...` (and future data subcommands) run without spinning up the LLM
    // stack.
    if (argc >= 2 && std::strcmp(argv[1], "chunk") == 0) {
        return data::cli_chunk(argc, argv);
    }
    if (argc >= 2 && std::strcmp(argv[1], "fetch") == 0) {
        return data::cli_fetch(argc, argv);
    }
    if (argc >= 2 && std::strcmp(argv[1], "hw") == 0) {
        return hardware::cli_hw(argc, argv);
    }
    if (argc >= 2 && std::strcmp(argv[1], "plan") == 0) {
        return hardware::cli_plan(argc, argv);
    }
    try {
        status::set_overall("starting", false);
        // Pull down any assets listed in data/sources.json that aren't
        // already in data/manifest.json. Blocking (so the pipeline loader
        // sees a populated data/ dir) but a no-op once the manifest is
        // complete. Errors on individual roles are non-fatal; the
        // downstream loader will refuse if its role isn't ready.
        try { data::bootstrap("data"); }
        catch (const std::exception & ex) {
            std::fprintf(stderr, "ac9: bootstrap warning: %s\n", ex.what());
        }
        // Enumerate GPUs, plan per-role offload, persist to
        // ~/.ac9/gpu_plan.json. Non-fatal on failure; loaders fall back
        // to their historical hardcoded assignments.
        try { hardware::init(); }
        catch (const std::exception & ex) {
            std::fprintf(stderr, "ac9: hardware init warning: %s\n", ex.what());
        }

        // Stop the server on signal so the shutdown path runs.
        std::signal(SIGINT,  [](int){ web_server::stop(); });
        std::signal(SIGTERM, [](int){ web_server::stop(); });

        // Spawn the pipeline loader; it updates status as it goes.
        std::thread loader(load_pipeline_background);
        loader.detach();

        // Block on the web server. The UI is reachable instantly; the
        // /api/chat handler will refuse if the models aren't ready yet
        // (it checks status).
        web_server::run("0.0.0.0", 8080);

        std::fprintf(stderr, "ac9: shutting down...\n");
        kb::shutdown();
        context::shutdown();
        return 0;
    } catch (const std::exception & ex) {
        std::fprintf(stderr, "ac9: %s\n", ex.what());
        return 1;
    }
}
