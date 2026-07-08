#pragma once

#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline void suite_ru20() {
    using nundertale::tiled_detail::JsonParser;
    using nundertale::tiled_detail::JsonValue;

    auto load = [](const std::string& id) {
        std::ifstream in("data/enemies/" + id + ".json", std::ios::binary);
        assert(in.good());
        std::stringstream ss;
        ss << in.rdbuf();
        JsonValue root;
        const std::string text = ss.str(); // the parser keeps a reference
        JsonParser parser(text);
        const bool ok = parser.parse(root);
        assert(ok);
        assert(root.kind == JsonValue::Kind::Object);
        return root;
    };
    auto str = [](const JsonValue& v, const std::string& key) {
        const JsonValue* f = v.field(key);
        return (f != nullptr && f->kind == JsonValue::Kind::String) ? f->string : std::string();
    };
    auto strings = [](const JsonValue& v, const std::string& key) {
        std::vector<std::string> out;
        const JsonValue* f = v.field(key);
        if (f != nullptr && f->kind == JsonValue::Kind::Array) {
            for (const JsonValue& e : f->array) {
                assert(e.kind == JsonValue::Kind::String);
                out.push_back(e.string);
            }
        }
        return out;
    };
    auto hasLine = [](const std::vector<std::string>& v, const std::string& s) {
        for (const auto& e : v) if (e == s) return true;
        return false;
    };

    struct Spec {
        std::string id;
        std::vector<std::string> acts;
        bool hug;
        std::string flees;
        std::string bubble;
    };
    const std::vector<Spec> specs = {
        { "froggsona", { "Check", "Compliment", "Threaten", "Accept Hug" }, true, "Threaten", "ribbit :3" },
        { "shysona", { "Check", "Console", "Terrorize", "Accept Hug" }, true, "Terrorize", "Sorry..." },
        { "modsmal", { "Check", "Imitate", "Flirt" }, false, "", "*squorch* (Read the pins.)" },
    };

    for (const Spec& spec : specs) {
        const JsonValue root = load(spec.id);
        assert(str(root, "id") == spec.id);
        assert(!str(root, "name").empty());
        assert(!str(root, "check").empty());
        assert(!str(root, "spareText").empty());
        assert(!str(root, "deathText").empty());

        // ACT names match the LORE, in order, each with text (Check's text is built by the engine)
        const JsonValue* acts = root.field("acts");
        assert(acts != nullptr && acts->kind == JsonValue::Kind::Array);
        assert(acts->array.size() == spec.acts.size());
        bool hasHug = false;
        for (std::size_t i = 0; i < spec.acts.size(); ++i) {
            const JsonValue& act = acts->array[i];
            assert(str(act, "label") == spec.acts[i]);
            assert(!str(act, "id").empty());
            if (i > 0) assert(!str(act, "text").empty());
            const JsonValue* fx = act.field("effects");
            assert(fx != nullptr && fx->kind == JsonValue::Kind::Object);
            if (spec.acts[i] == "Accept Hug") {
                hasHug = true;
                const JsonValue* spareNow = fx->field("spareNow");
                const JsonValue* fur = fx->field("fur");
                assert(spareNow != nullptr && spareNow->boolean);
                assert(fur != nullptr && static_cast<int>(fur->number) == 10);
            }
            if (spec.acts[i] == spec.flees) {
                const JsonValue* flees = fx->field("flees");
                assert(flees != nullptr && flees->boolean);
            }
        }
        assert(hasHug == spec.hug);

        // flavor buckets and bubbles
        const JsonValue* flavor = root.field("flavor");
        assert(flavor != nullptr);
        assert(strings(*flavor, "default").size() >= 3);
        assert(!strings(*flavor, "lowHp").empty());
        assert(!strings(*flavor, "spareable").empty());
        for (const auto& pool : { "default", "lowHp", "spareable" })
            for (const auto& line : strings(*flavor, pool)) assert(line.rfind("* ", 0) == 0);

        const JsonValue* dialogue = root.field("dialogue");
        assert(dialogue != nullptr);
        assert(hasLine(strings(*dialogue, "default"), spec.bubble));
        for (const auto& pool : { "afterHit", "crit", "miss", "item" })
            assert(!strings(*dialogue, pool).empty());
        assert(str(root, "spareText") == spec.bubble);

        const JsonValue* patterns = root.field("patterns");
        assert(patterns != nullptr && !patterns->array.empty());
    }

    // LORE §18.1: the Froggsona thought line, and the full bubble sets
    const JsonValue frog = load("froggsona");
    assert(hasLine(strings(*frog.field("flavor"), "default"), "* (It keeps staring at your ears.)"));
    assert(strings(*frog.field("dialogue"), "default").size() == 2);
    const JsonValue shy = load("shysona");
    assert(hasLine(strings(*shy.field("dialogue"), "default"), "Sorry for the art..."));
    assert(hasLine(strings(*shy.field("dialogue"), "default"), "You weren't supposed to see that..."));
    const JsonValue mod = load("modsmal");
    assert(hasLine(strings(*mod.field("dialogue"), "default"), "*squelch* (Tag your art.)"));

    std::cout << "[PASS] C1-020/NAR-006: Froggsona, Shysona and Modsmal Check, ACTs, thought, flavor and bubbles are complete." << std::endl;
}
