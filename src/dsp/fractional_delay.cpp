#include <chord/dsp/fractional_delay.hpp>
#include <cmath>
#include <kfr/math/interpolation.hpp>

namespace chord::dsp {

FractionalDelayState make_fractional_delay_state(size_t max_delay) {
    FractionalDelayState state;
    // Buffer needs to be large enough for the delay plus the interpolation kernel size.
    // For cubic (4-point), we need safety margin for wrapping and lookahead.
    state.size = max_delay + 8;
    state.buffer.resize(state.size, 0.0f);
    state.write_idx = 0;
    return state;
}

namespace {

inline size_t wrap_idx(intptr_t idx, size_t size) {
    while (idx < 0)
        idx += size;
    return static_cast<size_t>(idx % size);
}

} // namespace

void fractional_delay(kfr::univector_ref<const float> in,
                      kfr::univector_ref<float> out,
                      kfr::univector_ref<const float> delays,
                      FractionalDelayState& state,
                      InterpolationType interp) {
    const size_t size = in.size();
    if (size == 0)
        return;

    for (size_t i = 0; i < size; ++i) {
        // 1. Write current sample to buffer
        state.buffer[state.write_idx] = in[i];

        const float d = delays[i];
        const float floor_d = std::floor(d);
        const float frac = d - floor_d;

        // k is the index of the floor delay sample
        const intptr_t k = static_cast<intptr_t>(state.write_idx) - static_cast<intptr_t>(floor_d);

        if (interp == InterpolationType::Linear) {
            // Linear between k (delay=floor) and k-1 (delay=floor+1)
            const float y1 = state.buffer[wrap_idx(k, state.size)];
            const float y2 = state.buffer[wrap_idx(k - 1, state.size)];
            out[i] = kfr::linear(frac, y1, y2);
        } else {
            /**
             * Hermite / Catmull-Rom interpolation across 4 points.
             * Points are centered around the interval [delay=floor, delay=floor+1].
             * x0 = k+1, x1 = k (mu=0), x2 = k-1 (mu=1), x3 = k-2
             */
            const float x0 = state.buffer[wrap_idx(k + 1, state.size)];
            const float x1 = state.buffer[wrap_idx(k, state.size)];
            const float x2 = state.buffer[wrap_idx(k - 1, state.size)];
            const float x3 = state.buffer[wrap_idx(k - 2, state.size)];

            // Evaluate P(frac) = a3*frac^3 + a2*frac^2 + a1*frac + a0
            // kfr::horner(x, c0, c1, c2, c3) = c3*x^3 + c2*x^2 + c1*x + c0
            const float a0 = x1;
            const float a1 = 0.5f * (x2 - x0);
            const float a2 = x0 - 2.5f * x1 + 2.0f * x2 - 0.5f * x3;
            const float a3 = 0.5f * (x3 - x0) - 1.5f * (x2 - x1);

            out[i] = kfr::horner(frac, a0, a1, a2, a3);
        }

        // 2. Advance write pointer
        state.write_idx = (state.write_idx + 1) % state.size;
    }
}

} // namespace chord::dsp
