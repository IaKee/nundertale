#pragma once

#include "core/PhoneQueue.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline void suite_ru44() {
    using namespace nundertale;

    PhoneQueue queue;
    assert(queue.empty());
    assert(!queue.enqueue(""));
    assert(queue.enqueue("r10_call"));
    assert(queue.enqueue("r10_call2"));
    assert(!queue.enqueue("r10_call"));
    assert(queue.size() == 2U);

    assert(!queue.takeNext({ true, false, false }).has_value());
    assert(!queue.takeNext({ false, true, false }).has_value());
    assert(!queue.takeNext({ false, false, true }).has_value());
    assert(queue.size() == 2U);

    const auto first = queue.takeNext({});
    assert(first.has_value() && *first == "r10_call");
    assert(queue.size() == 1U);
    assert(queue.contains("r10_call2"));

    // The active first call keeps the second queued; it plays only after control is free.
    assert(!queue.takeNext({ true, false, false }).has_value());
    const auto second = queue.takeNext({});
    assert(second.has_value() && *second == "r10_call2");
    assert(queue.empty());

    std::ifstream file("data/cutscenes/r10_call2.json");
    assert(file.good());
    std::ostringstream json;
    json << file.rdbuf();
    const std::string call = json.str();
    assert(call.find("I put something in your closet. Just in case. No pressure!") != std::string::npos);
    assert(call.find("ruins.r10.call2") != std::string::npos);
    assert(call.find("assets/sfx/phone_buzz.ogg") != std::string::npos);

    std::cout << "[PASS] MNU-006 / C1-010: phone messages are FIFO, wait while busy, and R10's closet call follows the glitter call." << std::endl;
}
