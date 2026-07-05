#include "council.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace council {
namespace {

// Tokenize to lowercase content words. Strips punctuation; keeps
// alphanumerics and underscores. Drops a small stoplist so common
// connectives don't dominate the Jaccard score.
std::unordered_set<std::string> token_bag(const std::string & text) {
    static const std::unordered_set<std::string> stop = {
        "the","a","an","and","or","but","of","in","on","at","to","from","by",
        "for","with","as","is","are","was","were","be","been","being","this",
        "that","these","those","it","its","into","not","no","do","does","did",
        "have","has","had","you","i","we","they","he","she","them","us","our",
        "their","my","your","if","then","than","so","because","while","when",
        "which","what","who","whom","how","why","there","here"
    };
    std::unordered_set<std::string> out;
    std::string cur;
    auto flush = [&]() {
        if (cur.empty()) return;
        if (stop.count(cur) == 0 && cur.size() > 1) out.insert(cur);
        cur.clear();
    };
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            cur.push_back((char)std::tolower((unsigned char)c));
        } else {
            flush();
        }
    }
    flush();
    return out;
}

double jaccard(const std::unordered_set<std::string> & a,
               const std::unordered_set<std::string> & b) {
    if (a.empty() && b.empty()) return 1.0;
    size_t inter = 0;
    const auto & small = a.size() < b.size() ? a : b;
    const auto & big   = a.size() < b.size() ? b : a;
    for (const auto & t : small) if (big.count(t)) ++inter;
    size_t un = a.size() + b.size() - inter;
    return un == 0 ? 0.0 : (double)inter / (double)un;
}

std::string render_prompt(std::string_view prompt, const Round & prior) {
    std::ostringstream os;
    os << prompt;
    if (!prior.contributions.empty()) {
        os << "\n\n--- previous round contributions ---\n";
        for (const auto & c : prior.contributions) {
            os << "[" << c.specialist_name << "] " << c.text << "\n";
        }
    }
    return os.str();
}

std::string synthesize(const Round & last_round, std::string_view prompt) {
    std::ostringstream os;
    os << "**Synthesis** for prompt: " << prompt << "\n\n";
    for (const auto & c : last_round.contributions) {
        os << "### " << c.specialist_name << "\n\n" << c.text << "\n\n";
    }
    return os.str();
}

}

double mean_pairwise_jaccard(const std::vector<std::string> & texts) {
    if (texts.size() < 2) return 0.0;
    std::vector<std::unordered_set<std::string>> bags;
    bags.reserve(texts.size());
    for (const auto & t : texts) bags.push_back(token_bag(t));
    double sum   = 0.0;
    size_t pairs = 0;
    for (size_t i = 0; i < bags.size(); ++i) {
        for (size_t j = i + 1; j < bags.size(); ++j) {
            sum += jaccard(bags[i], bags[j]);
            ++pairs;
        }
    }
    return pairs == 0 ? 0.0 : sum / (double)pairs;
}

Verdict run(std::string_view                   prompt,
            const std::vector<Specialist>    & specialists,
            int                                max_rounds,
            double                             convergence_threshold) {
    if (specialists.empty())
        throw std::runtime_error("council::run: no specialists supplied");
    if (max_rounds < 1) max_rounds = 1;
    if (convergence_threshold < 0.0) convergence_threshold = 0.0;
    if (convergence_threshold > 1.0) convergence_threshold = 1.0;

    Verdict v;
    Round   prior;

    for (int r = 0; r < max_rounds; ++r) {
        Round cur;
        std::string rendered = render_prompt(prompt, prior);
        std::vector<std::string> bodies;
        bodies.reserve(specialists.size());

        for (const auto & sp : specialists) {
            std::string body = sp.generate(rendered);
            cur.contributions.push_back({sp.name, body});
            bodies.push_back(body);
        }
        cur.convergence = mean_pairwise_jaccard(bodies);
        v.rounds.push_back(cur);

        if (cur.convergence >= convergence_threshold) {
            v.converged         = true;
            v.final_convergence = cur.convergence;
            v.synthesis         = synthesize(cur, prompt);
            return v;
        }
        prior = cur;
    }

    const Round & last = v.rounds.back();
    v.final_convergence = last.convergence;
    v.converged         = false;
    v.synthesis         = synthesize(last, prompt);
    return v;
}

}
