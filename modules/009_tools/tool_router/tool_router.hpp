// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <string_view>
#include <vector>

// LLM-based tool router + prompt shaper.
//
// Replaces per-tool regex intent detectors (detect_ticket_intent,
// detect_image_gen_intent, detect_image_edit_intent, the parts-search
// keyword sniffer, ...) with one call into a small classifier model
// (qwen35 or planner-4b). The router reads the cleaned user prompt +
// optional wiki / dictionary blocks and emits strict JSON naming which
// registered tool to invoke, what args to pass, how confident it is,
// and why. Downstream at the /api/chat dispatch level:
//
//   confidence >= 0.7 -> dispatch to that tool with the shaped prompt
//   confidence <  0.7 -> fall through to the existing regex routers as
//                        a safety net (they usually catch crystal-clear
//                        cases like a bare "T-3" or "*.png" reference)
//   tool == "none"    -> fall through to classify::analyze + understanding
//                        stack + normal act dispatch
//
// The shaper is the second half of the same design: each tool in the
// registry carries a prompt_template describing the IDEAL shape of the
// LLM input for that tool (analog to the "one sprite = one ticket, no
// tool names, save to assets/<name>.png" rules the operator has been
// hand-typing into ticket_plan requests). When the router picks a tool,
// the shaper substitutes {user_prompt} into that template and returns
// the system + user message pair that the tool's downstream LLM should
// receive.
//
// This module never dispatches anything itself; it produces a Choice
// object. The chat pipeline in server.cpp does the actual invocation.
namespace tool_router {

// One tool the router can pick. Registered at init.
struct Tool {
    std::string name;             // e.g. "ticket_plan", "image_gen"
    std::string description;      // one-line for the router system prompt
    std::string args_schema;      // one-line JSON-shape hint, e.g.
                                  //   R"({"goal": "<string>"})"
    std::string prompt_template;  // shape rules for the tool's downstream
                                  // LLM call. Supports {user_prompt},
                                  // {goal}, {subject}, {id}, etc. as
                                  // simple `{name}` substitutions filled
                                  // from Choice.args plus the raw prompt.
};

// One dispatch decision produced by the router.
struct Choice {
    std::string    tool;          // one of the registered tools OR "none"
    nlohmann::json args;          // filled by router; may be object() when empty
    double         confidence;    // 0.0 .. 1.0
    std::string    reason;        // rationale from the router
    // Populated by shape() (or route_and_shape when the pick is
    // dispatch-worthy). Empty for tool == "none" or when shape() has
    // not been invoked yet.
    std::string    shaped_system; // system prompt for the tool's LLM
    std::string    shaped_user;   // user message for the tool's LLM
};

// Default tool registry. Called by init() but exposed here so a caller
// (e.g. a test) can reset the registry.
void register_defaults();

// Add or replace a tool by name.
void register_tool(Tool t);

// Immutable view of the current registry.
const std::vector<Tool> & registry();

// Look up a registered tool by name; nullptr on miss.
const Tool * find(std::string_view name);

// Ask the LLM to classify the user's intent into one of the registered
// tools. Returns Choice with tool="none" + confidence=0 on parse
// failure, model unavailability, or when the router genuinely cannot
// decide. Never throws. `wiki_block` and `dict_block` are OPTIONAL
// context strings from the info-gathering layers (entities lookup,
// dictionary, wikipedia); pass empty when they haven't run yet.
Choice route(const std::string & cleaned_prompt,
             const std::string & wiki_block  = "",
             const std::string & dict_block  = "");

// Apply a tool's prompt_template to the user's raw prompt + the router-
// extracted args. Populates c.shaped_system and c.shaped_user in place.
// Silently no-ops when tool == "none" or unknown.
void shape(Choice & c, const std::string & raw_user_prompt);

// Convenience: route + shape in one call, skipping the shape step when
// confidence is below `confidence_threshold`. The pipeline typically
// calls this and inspects the returned Choice.
Choice route_and_shape(const std::string & cleaned_prompt,
                       const std::string & raw_user_prompt,
                       double confidence_threshold = 0.7,
                       const std::string & wiki_block = "",
                       const std::string & dict_block = "");

// Populate the default registry (called once at server start). Safe to
// call multiple times.
void init();

}  // namespace tool_router
