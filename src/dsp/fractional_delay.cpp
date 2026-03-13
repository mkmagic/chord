#include <chord/dsp/fractional_delay.hpp>
#include <cmath>
#include <kfr/math/interpolation.hpp>

namespace chord::dsp {

FractionalDelayState make_fractional_delay_state(size_t max_delay) {
    FractionalDelayState state;
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
        state.buffer[state.write_idx] = in[i];

        const float d = delays[i];
        const float floor_d = std::floor(d);
        const float frac = d - floor_d;
        
        // k is the index of the floor delay sample
        const intptr_t k = static_cast<intptr_t>(state.write_idx) - static_cast<intptr_t>(floor_d);

        if (interp == InterpolationType::Linear) {
            const float y0 = state.buffer[wrap_idx(k, state.size)];
            const float y1 = state.buffer[wrap_idx(k - 1, state.size)];
            out[i] = kfr::linear(frac, y0, y1);
        } else {
            /**
             * Hermite / Cubic interpolation across 4 points.
             * Points are centered around the desired fractional position.
             * mu=0 -> x1 (k), mu=1 -> x2 (k-1)
             */
            const float x0 = state.buffer[wrap_idx(k + 1, state.size)];
            const float x1 = state.buffer[wrap_idx(k, state.size)];
            const float x2 = state.buffer[wrap_idx(k - 1, state.size)];
            const float x3 = state.buffer[wrap_idx(k - 2, state.size)];
            
            // Re-implementing the Hermite kernel manually to ensure index order is absolute
            // y = y1 + 0.5 * x * (y2 - y0 + x * (2.0*y0 - 5.0*y1 + 4.0*y2 - y3 + x * (3.0*(y1 - y2) + y3 - y0)))
            out[i] = x1 + 0.5f * frac * (x2 - x0 + frac * (2.0f * x0 - 5.0f * x1 + 4.0f * x2 - x3 + frac * (3.0f * (x1 - x2) + x3 - x0)));
        }

        state.write_idx = (state.write_idx + 1) % state.size;
    }
}

} // namespace chord::dsp
