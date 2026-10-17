#pragma once

#include <algorithm>

namespace nundertale {

struct ResidualMeter {
    float pending = 0.0f;        // HP-equivalent overlay still to drain
    float drainPerSecond = 1.0f; // KR-like: about 1 HP per second (confirm)
    float carry = 0.0f;          // fractional accumulator

    void add(float amount) {
        if (amount <= 0.0f) {
            return;
        }
        pending += amount;
    }

    int update(float dt) {
        if (dt <= 0.0f || pending <= 0.0f) {
            return 0;
        }

        float toDrain = std::min(pending, drainPerSecond * dt);
        pending -= toDrain;
        if (pending < 0.0f) {
            pending = 0.0f;
        }

        carry += toDrain;
        int whole = static_cast<int>(carry + 1e-5f);
        carry -= static_cast<float>(whole);
        if (carry < 0.0f) {
            carry = 0.0f;
        }
        return whole;
    }

    void clear() {
        pending = 0.0f;
        carry = 0.0f;
    }
};

} // namespace nundertale
