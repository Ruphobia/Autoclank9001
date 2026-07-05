// tool — main entry. Brings up the web UI immediately, then loads the
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
#include "010_interface/status.hpp"

#include <chrono>
#include <csignal>
#include <cstdio>
#include <exception>
#include <thread>

namespace {

void load_pipeline_background() {
    try {
        status::set_overall("opening session memory", false);
        status::note("context", "loading");
        context::init();
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
        std::fprintf(stderr, "tool: pipeline load error: %s\n", ex.what());
    }
}

}

int main() {
    try {
        status::set_overall("starting", false);

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

        std::fprintf(stderr, "tool: shutting down...\n");
        kb::shutdown();
        context::shutdown();
        return 0;
    } catch (const std::exception & ex) {
        std::fprintf(stderr, "tool: %s\n", ex.what());
        return 1;
    }
}
