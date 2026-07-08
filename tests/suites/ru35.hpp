#pragma once

#include "core/FurReactions.hpp"
#include "ui/DialogBox.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

inline void suite_ru35() {
    using namespace nundertale;

    // 1. Generic NPC FUR reaction: threshold 19 vs 20
    {
        assert(furReactionLine(0, "generic").empty());
        assert(furReactionLine(10, "generic").empty());
        assert(furReactionLine(19, "generic").empty());
        assert(furReactionLine(20, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(21, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(39, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(40, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(79, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(80, "generic") == "* Nice ears, newcomer!");
        assert(furReactionLine(100, "generic") == "* Nice ears, newcomer!");
    }

    // 2. Preserve existing Barky and unknown speaker behavior
    {
        assert(furReactionLine(19, "barky").empty());
        assert(furReactionLine(20, "barky").empty());
        assert(furReactionLine(39, "barky").empty());
        assert(furReactionLine(40, "barky") == "Dude. Your paws are SO much better than mine.");
        assert(furReactionLine(79, "barky") == "Dude. Your paws are SO much better than mine.");
        assert(furReactionLine(80, "barky") == "Dude. Your paws are SO much better than mine.");
        assert(furReactionLine(0, "unknown").empty());
        assert(furReactionLine(20, "unknown").empty());
        assert(furReactionLine(80, "unknown").empty());
        assert(furReactionLine(100, "unknown").empty());
    }

    // 3. Grub thought squeak: threshold 79 vs 80
    {
        const std::string thought = "* (Forbidden candy.)";
        assert(furReactionLine(0, thought).empty());
        assert(furReactionLine(20, thought).empty());
        assert(furReactionLine(78, thought).empty());
        assert(furReactionLine(79, thought).empty());
        assert(furReactionLine(80, thought) == " (squeak)");
        assert(furReactionLine(81, thought) == " (squeak)");
        assert(furReactionLine(99, thought) == " (squeak)");
        assert(furReactionLine(100, thought) == " (squeak)");

        // Named thought speakers also trigger the suffix at FUR >= 80
        assert(furReactionLine(79, "thought").empty());
        assert(furReactionLine(80, "thought") == " (squeak)");
        assert(furReactionLine(79, "pepe_thought").empty());
        assert(furReactionLine(80, "pepe_thought") == " (squeak)");
        assert(furReactionLine(79, "grub_thought").empty());
        assert(furReactionLine(80, "grub_thought") == " (squeak)");
    }

    // 4. Thought detection
    {
        // Valid thoughts
        assert(isThoughtLine("* (A small mound of gray fur and gamer dust.)"));
        assert(isThoughtLine("* (It smells faintly of regret and Doritos.)"));
        assert(isThoughtLine("* (The northern gate stands open.)"));
        assert(isThoughtLine("* (You touch your head. Nothing there.)"));
        assert(isThoughtLine("* (Nested (parens) work too.)"));
        assert(isThoughtLine("* (...)"));
        assert(isThoughtLine("   * (Leading spaces before asterisk)"));
        assert(isThoughtLine("\t* (Leading tab before asterisk)"));

        // Non-thoughts
        assert(!isThoughtLine("* Nice ears, newcomer!"));
        assert(!isThoughtLine("* A bottle. It's not Mountain Dew."));
        assert(!isThoughtLine("Dude. Your paws are SO much better than mine."));
        assert(!isThoughtLine("No asterisk (parentheses)"));
        assert(!isThoughtLine("* (Unclosed parenthesis"));
        assert(!isThoughtLine("*(No space after asterisk)"));
        assert(!isThoughtLine(""));

        // At FUR >= 80, only thought lines return the squeak
        assert(furReactionLine(80, "* (A valid thought.)") == " (squeak)");
        assert(furReactionLine(80, "* Not a thought (missing opening paren at start)").empty());
        assert(furReactionLine(80, "No asterisk (thought)").empty());
        assert(furReactionLine(80, "* (Unclosed thought").empty());
        assert(furReactionLine(80, "* A bottle. It's not Mountain Dew.").empty());
        assert(furReactionLine(80, "").empty());
    }

    // 5. No double squeak
    {
        const std::string unsqueaked = "* (You feel like scum.)";
        const std::string squeaked = "* (You feel like scum.) (squeak)";

        assert(!hasThoughtSqueak(unsqueaked));
        assert(hasThoughtSqueak(squeaked));

        // furReactionLine returns empty if squeak already present
        assert(furReactionLine(80, unsqueaked) == " (squeak)");
        assert(furReactionLine(80, squeaked).empty());

        // applyThoughtSqueak is idempotent
        std::string res1 = applyThoughtSqueak(unsqueaked, 80);
        assert(res1 == squeaked);
        std::string res2 = applyThoughtSqueak(res1, 80);
        assert(res2 == squeaked);

        // Multi-line page idempotency
        const std::string multiPage = "* (First line.)\n* (Second line.)";
        std::string multiRes1 = applyThoughtSqueakToPage(multiPage, 80);
        assert(multiRes1 == "* (First line.) (squeak)\n* (Second line.) (squeak)");
        std::string multiRes2 = applyThoughtSqueakToPage(multiRes1, 80);
        assert(multiRes2 == "* (First line.) (squeak)\n* (Second line.) (squeak)");

        // Mixed page with dialogue and thought
        const std::string mixedPage = "* A bottle. It's not Mountain Dew.\n* (You don't want to talk about it.)";
        std::string mixedRes1 = applyThoughtSqueakToPage(mixedPage, 80);
        assert(mixedRes1 == "* A bottle. It's not Mountain Dew.\n* (You don't want to talk about it.) (squeak)");
        std::string mixedRes2 = applyThoughtSqueakToPage(mixedRes1, 80);
        assert(mixedRes2 == "* A bottle. It's not Mountain Dew.\n* (You don't want to talk about it.) (squeak)");
    }

    // 6. DialogBox integration
    {
        // 6a. FUR 79: no squeak
        DialogBox box79;
        box79.startDialog({ "* (You consider drinking from the bottle.)" }, "", nullptr, "", 79);
        assert(box79.getPages().size() == 1);
        assert(box79.getPages()[0] == "* (You consider drinking from the bottle.)");

        // 6b. FUR 80: squeak appended
        DialogBox box80;
        box80.startDialog({ "* (You consider drinking from the bottle.)" }, "", nullptr, "", 80);
        assert(box80.getPages().size() == 1);
        assert(box80.getPages()[0] == "* (You consider drinking from the bottle.) (squeak)");

        // 6c. Non-thought lines in DialogBox do not get squeak even at FUR 80
        DialogBox boxNpc;
        boxNpc.startDialog({ "* Nice ears, newcomer!" }, "generic", nullptr, "", 80);
        assert(boxNpc.getPages().size() == 1);
        assert(boxNpc.getPages()[0] == "* Nice ears, newcomer!");

        // 6d. No double squeak inside DialogBox
        DialogBox boxNoDouble;
        boxNoDouble.startDialog({ "* (Already squeaked.) (squeak)" }, "", nullptr, "", 80);
        assert(boxNoDouble.getPages().size() == 1);
        assert(boxNoDouble.getPages()[0] == "* (Already squeaked.) (squeak)");

        // 6e. Calling setFur on active DialogBox applies squeak
        DialogBox boxSetFur;
        boxSetFur.startDialog({ "* (Just thinking.)" });
        assert(boxSetFur.getPages()[0] == "* (Just thinking.)");
        boxSetFur.setFur(79);
        assert(boxSetFur.getPages()[0] == "* (Just thinking.)");
        boxSetFur.setFur(80);
        assert(boxSetFur.getPages()[0] == "* (Just thinking.) (squeak)");
        // Calling setFur again does not double-squeak
        boxSetFur.setFur(85);
        assert(boxSetFur.getPages()[0] == "* (Just thinking.) (squeak)");

        // 6f. Multi-line page in DialogBox
        DialogBox boxMulti;
        boxMulti.startDialog({ "* A bottle. It's not Mountain Dew.\n* (You don't want to talk about it.)" }, "", nullptr, "", 80);
        assert(boxMulti.getPages().size() == 1);
        assert(boxMulti.getPages()[0] == "* A bottle. It's not Mountain Dew.\n* (You don't want to talk about it.) (squeak)");
    }

    std::cout << "[PASS] FUR-008: generic NPC FUR reactions and Grub squeak at FUR >= 80 verified." << std::endl;
}
