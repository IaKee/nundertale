#pragma once

#include "core/TiledMap.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline void suite_ru21() {
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
        std::string name;
        std::string check;
        std::vector<std::string> acts;
        std::vector<std::string> bubbles;
        std::string spare;
    };
    const std::vector<Spec> specs = {
        { "lurkeye", "Lurkeye",
          "Has watched the 18+ channel since 2016. Has never typed. Not once.",
          { "Check", "Don't Pick On", "Pick On" },
          { "...", "(typing...)", "(stopped typing)" }, "..." },
        { "reply_guy", "Reply Guy",
          "Has a correction about your fursona's species.",
          { "Check", "Talk" },
          { "Well, actually, that's a coyote.", "Source?", "...yeah, it's a wolf, man." },
          "...yeah, it's a wolf, man." },
        { "grasstoid", "Grasstoid",
          "A living patch of grass. Nobody down here has touched it. Ever.",
          { "Check", "Touch", "Dinner" },
          { "Touch me.", "Please. Nobody ever does.", "Outside is NICE." }, "Outside is NICE." },
    };

    for (const Spec& spec : specs) {
        const JsonValue root = load(spec.id);
        assert(str(root, "id") == spec.id);
        assert(str(root, "name") == spec.name);
        assert(str(root, "check") == spec.check);
        assert(!str(root, "spareText").empty());
        assert(str(root, "spareText") == spec.spare);
        assert(!str(root, "deathText").empty());

        const JsonValue* acts = root.field("acts");
        assert(acts != nullptr && acts->kind == JsonValue::Kind::Array);
        assert(acts->array.size() == spec.acts.size());
        for (std::size_t i = 0; i < spec.acts.size(); ++i) {
            const JsonValue& act = acts->array[i];
            assert(str(act, "label") == spec.acts[i]);
            assert(!str(act, "id").empty());
            if (i > 0) assert(!str(act, "text").empty()); // Check's text is built by the engine
            const JsonValue* fx = act.field("effects");
            assert(fx != nullptr && fx->kind == JsonValue::Kind::Object);
        }

        const JsonValue* flavor = root.field("flavor");
        assert(flavor != nullptr);
        assert(strings(*flavor, "default").size() >= 3);
        assert(!strings(*flavor, "lowHp").empty());
        assert(!strings(*flavor, "spareable").empty());
        for (const auto& pool : { "default", "lowHp", "spareable" })
            for (const auto& line : strings(*flavor, pool)) assert(line.rfind("* ", 0) == 0);

        const JsonValue* dialogue = root.field("dialogue");
        assert(dialogue != nullptr);
        for (const auto& b : spec.bubbles) assert(hasLine(strings(*dialogue, "default"), b));
        for (const auto& pool : { "afterHit", "crit", "miss", "item" })
            assert(!strings(*dialogue, pool).empty());

        const JsonValue* patterns = root.field("patterns");
        assert(patterns != nullptr && !patterns->array.empty());
    }

    // Lurkeye: Pick On is spareable and boosts ATK once; Don't Pick On is spareable
    {
        const JsonValue lurk = load("lurkeye");
        const JsonValue& pick = lurk.field("acts")->array[2];
        const JsonValue* fx = pick.field("effects");
        assert(fx->field("spareable") != nullptr && fx->field("spareable")->boolean);
        assert(fx->field("atkDeltaOnce") != nullptr && fx->field("atkDeltaOnce")->boolean);
        assert(fx->field("atkDelta") != nullptr && static_cast<int>(fx->field("atkDelta")->number) == 2);
        const JsonValue* kind = lurk.field("acts")->array[1].field("effects")->field("spareable");
        assert(kind != nullptr && kind->boolean);
    }

    // Reply Guy: Talk is the spare, and the "only alone" condition is recorded
    {
        const JsonValue reply = load("reply_guy");
        const JsonValue& talk = reply.field("acts")->array[1];
        assert(talk.field("effects")->field("spareable")->boolean);
        assert(hasLine(strings(reply, "aloneOnly"), "talk"));
        assert(!str(reply, "aloneNote").empty());
    }

    // Grasstoid: Touch is FUR -10 and spares; Dinner makes the greens heal next turn
    {
        const JsonValue grass = load("grasstoid");
        const JsonValue* touchFx = grass.field("acts")->array[1].field("effects");
        assert(static_cast<int>(touchFx->field("fur")->number) == -10);
        assert(touchFx->field("spareable")->boolean);
        const JsonValue* dinnerFx = grass.field("acts")->array[2].field("effects");
        assert(dinnerFx->field("healNextAttack")->boolean);
        const JsonValue& pat = grass.field("patterns")->array[0];
        assert(str(pat, "id") == "grass_blades");
        assert(static_cast<int>(pat.field("params")->field("heal")->number) > 0);
    }

    std::cout << "[PASS] C1-020/NAR-006: Lurkeye, Reply Guy and Grasstoid Check, ACTs, flavor, bubbles and spare/flee data are complete." << std::endl;
}
