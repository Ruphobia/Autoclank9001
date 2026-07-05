#include "leaderboard_scraper.hpp"

namespace leaderboard_scraper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Leaderboard Scraper (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
